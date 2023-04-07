#pragma once

#include <assert.h>

#include <queue>
#include <string>
#include <vector>

#include "b_plus_tree_page.hpp"
#include "page.h"
namespace huang {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType>

/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain actual data.
 * (1) We only support unique key
 * (2) support insert & remove
 * (3) The structure should shrink and grow dynamically
 * (4) Implement index iterator for range scan
 */
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
    using BPlusLeafPage = BPlusTreePage<KeyType, ValueType>;
    using BPlusInternalPage = BPlusTreePage<KeyType, page_id_t>;
    enum POS { LEFT, RIGHT };

    template <class N>
    struct GetSiblingAns {
        BPlusTreePage<KeyType, N> *page;
        bool ifavail;
        POS pos;
    };

   public:
    explicit BPlusTree(BufferPoolManager *buffer_pool_manager)
        : buffer_pool_manager_(buffer_pool_manager) {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        size_ = header_page->size;
        buffer_pool_manager_->disk_manager_->next_page_id_ =
            header_page->allocator;
    };

    ~BPlusTree() {
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
        header_page->size = size_;
        header_page->allocator =
            buffer_pool_manager_->disk_manager_->next_page_id_;
    }

    // Returns true if this B+ tree has no keys and values.
    bool IsEmpty() const { return !size_; };

    void Debug() {
        BPlusInternalPage *p = FetchRoot();
        if (p->IsLeafPage()) {
            BPlusLeafPage *i = reinterpret_cast<BPlusLeafPage *>(p);
            debug(i);
        } else
            debug(p);
    }

    template <class N>
    void print(BPlusTreePage<KeyType, N> *page) {
        if (page->IsLeafPage()) {
            std::cout << "leaf :" << page->page_id_
                      << " parent:" << page->parent_page_id_ << std::endl;
            for (int i = 0; i < page->size_; i++)
                std::cout << page->data_[i].first << " ";
            std::cout << std::endl;
            for (int i = 0; i < page->size_; i++)
                std::cout << page->data_[i].second << " ";
            std::cout << std::endl;
        } else {
            std::cout << "int :" << page->page_id_
                      << " parent:" << page->parent_page_id_ << std::endl;
            for (int i = 0; i <= page->size_; i++)
                std::cout << page->data_[i].first << " ";
            std::cout << std::endl;
            for (int i = 0; i <= page->size_; i++)
                std::cout << page->data_[i].second << " ";
            std::cout << std::endl;
        }
    }
    template <class N>
    void debug(BPlusTreePage<KeyType, N> *page) {
        print(page);
        if (!page->IsLeafPage() && page->size_) {
            for (int i = 0; i <= page->size_; i++) {
                BPlusInternalPage *p = reinterpret_cast<BPlusInternalPage *>(
                    buffer_pool_manager_->FetchPage(page->data_[i].second)
                        ->GetData());
                if (p->IsLeafPage()) {
                    BPlusLeafPage *i = reinterpret_cast<BPlusLeafPage *>(p);
                    debug(i);
                } else
                    debug(p);
            }
        }
    }

    // Insert a key-value pair into this B+ tree.
    void Insert(const KeyType &key, const ValueType &value) {
        if (IsEmpty())
            StartNewTree(key, value);
        else
            InsertIntoLeaf(key, value);
        size_++;
    };

    // Remove a key and its value from this B+ tree.
    bool Remove(const KeyType &key) {
        BPlusLeafPage *leaf_page = FindPos(key);
        RemoveFromPage(leaf_page, key);
        size_--;
        return true;
    };

    // return the value according to the key provided
    std::pair<ValueType *, bool> GetValue(const KeyType &key) {
        BPlusLeafPage *pg = FindPos(key);
        for (int i = 0; i < pg->size_; i++)
            if (pg->data_[i].first == key) return {&pg->data_[i].second, true};
        return {nullptr, false};
    };

    // return the value between two keys
    void GetValue(const KeyType &first_key, const KeyType &last_key,
                  std::vector<ValueType> *result) {
        BPlusLeafPage *st = FindPos(first_key), *ed = FindPos(last_key),
                      *nxt = st;
        for (int ind = st->page_id_; ind != ed->page_id_;) {
            int i;
            for (i = 0; i < nxt->size_; i++)
                if (first_key <= nxt->data_[i].first) break;
            for (int j = i; j < nxt->size_; j++) result->push_back(nxt->data_[j].second);
            nxt = reinterpret_cast<BPlusLeafPage *>(
                buffer_pool_manager_->FetchPage(nxt->nxt)->GetData());
            ind = nxt->page_id_;
        }
        int i;
        for (i = 0; i < nxt->size_; i++)
            if (first_key <= nxt->data_[i].first) break;
        for (int j = i; j < nxt->size_ && nxt->data_[j].first <= last_key; j++)
            result->push_back(nxt->data_[j].second);
    };

   private:
    HeaderPage *FetchHead() {
        return reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->FetchPage(0)->GetData());
    }
    BPlusInternalPage *FetchRoot() {
        HeaderPage *head = FetchHead();
        return reinterpret_cast<BPlusInternalPage *>(
            buffer_pool_manager_->FetchPage(head->root)->GetData());
    }
    BPlusLeafPage *FindPos(const KeyType &key) {
        BPlusInternalPage *page = FetchRoot();
        while (!page->IsLeafPage()) {
            int i;
            for (i = 0; i <= page->size_; i++)
                if (key < page->data_[i].first || i == page->size_) {
                    page = reinterpret_cast<BPlusInternalPage *>(
                        buffer_pool_manager_->FetchPage(page->data_[i].second)
                            ->GetData());
                    break;
                }
        }
        return reinterpret_cast<BPlusLeafPage *>(page);
    }
    template <class N>
    GetSiblingAns<N> FetchSibling(BPlusTreePage<KeyType, N> *page) {
        BPlusTreePage<KeyType, N> *p =
            reinterpret_cast<BPlusTreePage<KeyType, N> *>(
                buffer_pool_manager_->FetchPage(page->parent_page_id_)
                    ->GetData());
        BPlusLeafPage *sibling = nullptr;
        GetSiblingAns<N> ret = {nullptr, false, LEFT};
        int i;
        for (i = 0; i <= p->size_; i++)
            if (p->data_[i].second == page->page_id_) break;
        if (i > p->size_) return ret;
        if (i > 0) {
            sibling = reinterpret_cast<BPlusTreePage<KeyType, N> *>(
                buffer_pool_manager_->FetchPage(p->data_[i - 1].second)
                    ->GetData());
            ret = {sibling, true, LEFT};
            if (sibling->size_ >= sibling->min_size + 1) return ret;
        }
        if (i < p->size_) {
            sibling = reinterpret_cast<BPlusTreePage<KeyType, N> *>(
                buffer_pool_manager_->FetchPage(p->data_[i + 1].second)
                    ->GetData());
            ret = {sibling, true, RIGHT};
            if (sibling->size_ >= sibling->min_size + 1) return ret;
        }
        ret.ifavail = false;
        return ret;
    }

    void UpdateParentKey(BPlusInternalPage *p, const KeyType &old_key,
                         const KeyType &new_key) {
        while (true) {
            int i;
            for (i = 0; i < p->size_; i++)
                if (p->data_[i].first == old_key) {
                    p->data_[i].first = new_key;
                    break;
                }
            if (p->is_root_)
                return;
            else
                p = reinterpret_cast<BPlusInternalPage *>(
                    buffer_pool_manager_->FetchPage(p->parent_page_id_)
                        ->GetData());
        }
    }

    void RemoveFromPage(BPlusLeafPage *page, const KeyType &key) {
        int i;
        for (i = 0; i < page->size_; i++)
            if (page->data_[i].first == key) break;
        if (i == page->size_) return;
        if (i == 0 && !page->is_root_) {
            BPlusInternalPage *p = reinterpret_cast<BPlusInternalPage *>(
                buffer_pool_manager_->FetchPage(page->parent_page_id_)
                    ->GetData());
            UpdateParentKey(p, page->data_[0].first, page->data_[1].first);
        }

        for (int j = i; j < page->size_ - 1; j++)
            page->data_[j] = page->data_[j + 1];
        page->size_--;
        if (page->is_root_) return;

        FixPage(page);
        // page = reinterpret_cast<BPlusLeafPage *>(
        //     buffer_pool_manager_->FetchPage(page->page_id_));

        while (!page->is_root_) {
            page = reinterpret_cast<BPlusInternalPage *>(
                buffer_pool_manager_->FetchPage(page->parent_page_id_)
                    ->GetData());
            FixPage(page);
            // UpdateRoot(page);
        }
    }

    template <class N>
    void FixPage(BPlusTreePage<KeyType, N> *&page) {
        if (page->size_ >= page->min_size) return;
        GetSiblingAns<N> sibling = FetchSibling(page);
        if (sibling.page == nullptr) return;
        if (sibling.ifavail) {
            if (sibling.pos == LEFT) {
                if (page->IsLeafPage()) {
                    for (int i = page->size_ - 1; i >= 0; i--)
                        page->data_[i + 1] = page->data_[i];
                    page->data_[0] =
                        sibling.page->data_[sibling.page->size_ - 1];
                    page->size_++;
                    sibling.page->size_--;
                    BPlusInternalPage *p =
                        reinterpret_cast<BPlusInternalPage *>(
                            buffer_pool_manager_
                                ->FetchPage(page->parent_page_id_)
                                ->GetData());
                    int i;
                    for (i = 0; i <= p->size_; i++)
                        if (p->data_[i].second == sibling.page->page_id_) break;
                    p->data_[i].first = page->data_[0].first;
                } else {
                    for (int i = page->size_; i >= 0; i--)
                        page->data_[i + 1] = page->data_[i];
                    page->data_[0] = sibling.page->data_[sibling.page->size_];
                    page->size_++;
                    sibling.page->size_--;
                    BPlusInternalPage *p =
                        reinterpret_cast<BPlusInternalPage *>(
                            buffer_pool_manager_
                                ->FetchPage(page->parent_page_id_)
                                ->GetData());
                    int i;
                    for (i = 0; i <= p->size_; i++)
                        if (p->data_[i].second == sibling.page->page_id_) break;
                    page->data_[0].first = p->data_[i].first;
                    p->data_[i].first =
                        sibling.page->data_[sibling.page->size_].first;
                }
            } else {
                if (page->IsLeafPage()) {
                    page->data_[page->size_] = sibling.page->data_[0];
                    for (int i = 1; i < sibling.page->size_; i++)
                        sibling.page->data_[i - 1] = sibling.page->data_[i];
                    page->size_++;
                    sibling.page->size_--;
                    BPlusInternalPage *p =
                        reinterpret_cast<BPlusInternalPage *>(
                            buffer_pool_manager_
                                ->FetchPage(page->parent_page_id_)
                                ->GetData());
                    int i;
                    for (i = 0; i <= p->size_; i++)
                        if (p->data_[i].second == sibling.page->page_id_) break;
                    p->data_[i - 1].first = sibling.page->data_[0].first;
                } else {
                    BPlusInternalPage *p =
                        reinterpret_cast<BPlusInternalPage *>(
                            buffer_pool_manager_
                                ->FetchPage(page->parent_page_id_)
                                ->GetData());
                    int i;
                    for (i = 0; i <= p->size_; i++)
                        if (p->data_[i].second == page->page_id_) break;
                    page->data_[page->size_].first = p->data_[i].first;
                    page->data_[++page->size_] = sibling.page->data_[0];
                    p->data_[i].first = sibling.page->data_[0].first;

                    for (int i = 1; i <= sibling.page->size_; i++)
                        sibling.page->data_[i - 1] = sibling.page->data_[i];
                    sibling.page->size_--;
                    // p->data_[i - 1].first = sibling.page->data_[0].first;
                }
            }
        } else {
            if (sibling.pos == LEFT) {
                auto tmp = sibling.page;
                sibling.page = page;
                page = tmp;
            }
            if (page->IsLeafPage()) {
                for (int i = 0; i < sibling.page->size_; i++)
                    page->data_[i + page->size_] = sibling.page->data_[i];
                page->size_ += sibling.page->size_;
                BPlusInternalPage *p = reinterpret_cast<BPlusInternalPage *>(
                    buffer_pool_manager_->FetchPage(page->parent_page_id_)
                        ->GetData());
                int i;
                for (i = 0; i <= p->size_; i++)
                    if (p->data_[i].second == page->page_id_) break;
                p->data_[i + 1].second = page->page_id_;
                for (int j = i + 1; j <= p->size_; j++)
                    p->data_[j - 1] = p->data_[j];
                p->size_--;

            } else {
                BPlusInternalPage *p = reinterpret_cast<BPlusInternalPage *>(
                    buffer_pool_manager_->FetchPage(page->parent_page_id_)
                        ->GetData());
                int i;
                for (i = 0; i <= p->size_; i++)
                    if (p->data_[i].second == page->page_id_) break;
                page->data_[page->size_++].first = p->data_[i].first;

                for (int j = i; j < p->size_; j++)
                    p->data_[j] = p->data_[j + 1];
                p->data_[i].second = page->page_id_;
                p->size_--;

                for (int i = 0; i <= sibling.page->size_; i++)
                    page->data_[i + page->size_] = sibling.page->data_[i];
                page->size_ += sibling.page->size_;

                if (p->size_ == 0 && p->is_root_) {
                    p->is_root_ = false;
                    page->is_root_ = true;
                    page->parent_page_id_ = -1;
                    UpdateRoot(page);
                    UpdateRootId(page->page_id_);
                }
            }
        }
        UpdateRoot(page);
    }

    void StartNewTree(const KeyType &key, const ValueType &value) {
        page_id_t header_page_id;
        HeaderPage *header_page = reinterpret_cast<HeaderPage *>(
            buffer_pool_manager_->NewPage(&header_page_id)->GetData());
        header_page->root = 1;

        page_id_t root_page_id;
        BPlusLeafPage *root_page = reinterpret_cast<BPlusLeafPage *>(
            buffer_pool_manager_->NewPage(&root_page_id)->GetData());
        root_page->Init(1, LEAF_PAGE, true);

        InsertIntoLeaf(key, value);
    };

    void InsertIntoLeaf(const KeyType &key, const ValueType &value) {
        BPlusLeafPage *insert_leaf = FindPos(key);
        insert_leaf->Insert(key, value);
        if (insert_leaf->size_ > insert_leaf->max_size) Split(insert_leaf);
    }

    void InsertIntoInt(const page_id_t &parent_page_id, const KeyType &key,
                       const page_id_t &page_id_left,
                       const page_id_t &page_id_right) {
        BPlusInternalPage *parent = reinterpret_cast<BPlusInternalPage *>(
            buffer_pool_manager_->FetchPage(parent_page_id)->GetData());
        int i;
        for (i = 0; i < parent->size_; i++)
            if (key < parent->data_[i].first) break;
        for (int j = parent->size_; j >= i; j--)
            parent->data_[j + 1] = parent->data_[j];
        parent->data_[i] = {key, page_id_left};
        parent->data_[i + 1].second = page_id_right;
        parent->size_++;
    }

    template <class N>
    void Split(BPlusTreePage<KeyType, N> *page) {
        if (page->size_ <= page->max_size) return;
        page_id_t new_page_id;
        BPlusTreePage<KeyType, N> *new_page =
            reinterpret_cast<BPlusTreePage<KeyType, N> *>(
                buffer_pool_manager_->NewPage(&new_page_id)->GetData());
        KeyType *key;
        if (page->IsLeafPage()) {
            new_page->Init(new_page_id, page->page_type_, false,
                           page->size_ - page->size_ / 2);
            key = &page->data_[page->size_ / 2].first;

            page->size_ /= 2;
            for (int i = page->size_; i <= page->size_ + new_page->size_ - 1;
                 i++)
                new_page->data_[i - page->size_] = page->data_[i];
        } else {
            new_page->Init(new_page_id, page->page_type_, false,
                           page->size_ - page->size_ / 2 - 1);
            key = &page->data_[page->size_ / 2].first;

            page->size_ /= 2;
            for (int i = page->size_ + 1;
                 i <= page->size_ + new_page->size_ + 1; i++)
                new_page->data_[i - page->size_ - 1] = page->data_[i];
        }
        new_page->nxt = page->nxt;
        new_page->parent_page_id_ = page->parent_page_id_;
        page->nxt = new_page_id;

        if (page->is_root_) {
            page_id_t new_root_id;
            BPlusTreePage<KeyType, page_id_t> *new_root =
                reinterpret_cast<BPlusTreePage<KeyType, page_id_t> *>(
                    buffer_pool_manager_->NewPage(&new_root_id)->GetData());
            new_root->Init(new_root_id, INTERNAL_PAGE, true);
            page->is_root_ = false;
            InsertIntoInt(new_root_id, *key, page->page_id_, new_page_id);
            UpdateRoot(new_page);
            UpdateRootId(new_root_id);

            page->parent_page_id_ = new_root_id;
            new_page->parent_page_id_ = new_root_id;
        } else {
            InsertIntoInt(page->parent_page_id_, *key, page->page_id_,
                          new_page->page_id_);
            UpdateRoot(new_page);

            new_page->parent_page_id_ = page->parent_page_id_;
            BPlusTreePage<KeyType, page_id_t> *pg =
                reinterpret_cast<BPlusTreePage<KeyType, page_id_t> *>(
                    buffer_pool_manager_->FetchPage(page->parent_page_id_)
                        ->GetData());
            Split(pg);
        }
    }

    void UpdateRoot(BPlusInternalPage *page) {
        if (page->IsLeafPage()) return;
        for (int i = 0; i <= page->size_; i++) {
            BPlusInternalPage *pg = reinterpret_cast<BPlusInternalPage *>(
                buffer_pool_manager_->FetchPage(page->data_[i].second)
                    ->GetData());
            if (pg->IsLeafPage()) {
                BPlusLeafPage *i = reinterpret_cast<BPlusLeafPage *>(pg);
                i->parent_page_id_ = page->page_id_;
            } else
                pg->parent_page_id_ = page->page_id_;
        }
    }
    void UpdateRootId(page_id_t root_id) {
        HeaderPage *head = FetchHead();
        head->root = root_id;
    }

    BufferPoolManager *buffer_pool_manager_;
    size_t size_;
};

}  // namespace huang
