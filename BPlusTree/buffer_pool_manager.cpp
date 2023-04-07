#include "include/buffer_pool_manager.h"

#include "include/b_plus_tree_page.hpp"

namespace huang {
BufferPoolManager::BufferPoolManager(size_t pool_size,
                                     DiskManager *disk_manager,
                                     Replacer<Page *> *replacer) {
    pool_size_ = pool_size;
    pages_ = new Page[pool_size_];
    disk_manager_ = disk_manager;
    replacer_ = replacer;
    for (int i = 0; i < pool_size_; i++) free_.push_back(&pages_[i]);
}
BufferPoolManager::~BufferPoolManager() {
    FlushAllPage();
    delete[] pages_;
}

Page *BufferPoolManager::FetchPage(page_id_t page_id) {
    try {
        return page_map_.at(page_id);
    } catch (...) {
        Page *pg = FindUnusedPage(page_id);
        disk_manager_->ReadPage(page_id, pg->data_);

        return pg;
    }
}
/*
 * Implementation of unpin page
 * if pin_count>0, decrement it and if it becomes zero, put it back to
 * replacer if pin_count<=0 before this call, return false. is_dirty: set the
 * dirty flag of this page
 */
// bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty){};

bool BufferPoolManager::FlushPage(page_id_t page_id) {
    Page *pg = page_map_[page_id];
    if (pg->is_dirty_) {
        pg->is_dirty_ = false;
        disk_manager_->WritePage(page_id, pg->data_);
    }
    return true;
};

Page *BufferPoolManager::NewPage(page_id_t *page_id) {
    *page_id = disk_manager_->AllocatePage();
    Page *pg = FindUnusedPage(*page_id);
    return pg;
};

bool BufferPoolManager::DeletePage(page_id_t page_id){

};

Page *BufferPoolManager::FindUnusedPage(const page_id_t &page_id) {
    Page *page;
    if (!free_.empty()) {
        page = free_.front();
        free_.pop_front();
    } else {
        if (!replacer_->Victim(page)) return nullptr;
        page_id_t old_page_id = page_id_map_[page];
        page_map_[old_page_id] = page;
        FlushPage(old_page_id);
        page_map_.erase(page_map_.find(old_page_id));
        page_id_map_.erase(page_id_map_.find(page));
        page->Clear();
    }
    replacer_->Insert(page);
    page_map_[page_id] = page;
    page_id_map_[page] = page_id;
    page->is_dirty_ = true;
    return page;
};

void BufferPoolManager::FlushAllPage() {
    for (auto it = page_map_.begin(); it != page_map_.end(); it++)
        FlushPage(it->first);
};

}  // namespace huang