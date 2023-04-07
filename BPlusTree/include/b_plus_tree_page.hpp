
#pragma once

#include <iostream>

#include "buffer_pool_manager.h"
#include "config.h"

namespace huang {

#define INDEX_TEMPLATE_ARGUMENTS template <typename KeyType, typename ValueType>

class HeaderPage {
   public:
    page_id_t root = 0;
    page_id_t allocator = 0;
    int size;
};

// define page type enum
enum IndexPageType { INVALID_INDEX_PAGE = 0, LEAF_PAGE, INTERNAL_PAGE };

template <class KeyType, class ValueType>
class BPlusTreePage {
   public:
    BPlusTreePage() {
        page_type_ = INVALID_INDEX_PAGE;
        is_root_ = false;
        parent_page_id_ = -1;
        page_id_ = -1;
        nxt = -1;
    };
    bool IsLeafPage() const { return page_type_ == LEAF_PAGE; };
    void Init(page_id_t page_id = 0, IndexPageType page_type = LEAF_PAGE,
              bool is_root = false, size_t size = 0) {
        page_id_ = page_id;
        nxt = parent_page_id_ = -1;
        page_type_ = page_type;
        is_root_ = is_root;
        size_ = size;
    }
    void Insert(const KeyType& key, const ValueType& value) {
        int i;
        for (i = 0; i < size_; i++)
            if (key < data_[i].first) break;
        for (int j = size_ - 1; j >= i; j--) data_[j + 1] = data_[j];
        data_[i] = {key, value};
        size_++;
    }
    static const int size =
        (PAGE_SIZE - sizeof(IndexPageType) - sizeof(size_t) - sizeof(bool) -
         6 * sizeof(page_id_t)) /
            sizeof(std::pair<KeyType, ValueType>) -
        1;
    static const int max_size = size - 1;
    static const int min_size = size / 2 - 1;
    std::pair<KeyType, ValueType> data_[size];

    IndexPageType page_type_ = INVALID_INDEX_PAGE;
    size_t size_ = 0;
    bool is_root_ = false;
    page_id_t parent_page_id_ = -1;
    page_id_t page_id_ = -1;
    page_id_t nxt = -1;
};

}  // namespace huang
