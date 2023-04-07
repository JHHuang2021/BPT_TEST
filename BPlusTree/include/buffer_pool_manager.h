#pragma once

#include <list>
#include <unordered_map>
// #include <mutex>  // NOLINT

#include "config.h"
#include "disk_manager.h"
#include "lru_replacer.hpp"
#include "page.h"

namespace huang {

/**
 * BufferPoolManager reads disk pages to and from its internal buffer pool.
 */
class BufferPoolManager {
   public:
    BufferPoolManager(size_t pool_size, DiskManager *disk_manager,
                      Replacer<Page *> *replacer);
    /**
     * Destroys an existing BufferPoolManager.
     */
    ~BufferPoolManager();

    /** @return size of the buffer pool */
    size_t GetPoolSize();
    /**
     * Fetch the requested page from the buffer pool.
     * @param page_id id of page to be fetched
     * @return the requested page
     */
    Page *FetchPage(page_id_t page_id);

    /**
     * Unpin the target page from the buffer pool.
     * @param page_id id of page to be unpinned
     * @param is_dirty true if the page should be marked as dirty, false
     * otherwise
     * @return false if the page pin count is <= 0 before this call, true
     * otherwise
     */
    bool UnpinPage(page_id_t page_id, bool is_dirty);

    /**
     * Flushes the target page to disk.
     * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
     * @return false if the page could not be found in the page table, true
     * otherwise
     */
    bool FlushPage(page_id_t page_id);

    /**
     * Creates a new page in the buffer pool.
     * @param[out] page_id id of created page
     * @return nullptr if no new pages could be created, otherwise pointer to
     * new page
     */
    Page *NewPage(page_id_t *page_id);

    /**
     * Deletes a page from the buffer pool.
     * @param page_id id of page to be deleted
     * @return false if the page exists but could not be deleted, true if the
     * page didn't exist or deletion succeeded
     */
    bool DeletePage(page_id_t page_id);

    /**
     * Flushes all the pages in the buffer pool to disk.
     */
    void FlushAllPage();
    
    DiskManager *disk_manager_;

   private:
    size_t pool_size_;  // number of pages in buffer pool
    Page *pages_;       // array of pages
    linked_hashmap<page_id_t, Page *> page_map_;
    linked_hashmap<Page *, page_id_t> page_id_map_;
    std::list<Page *> free_;      // to be modified
    Replacer<Page *> *replacer_;  // to find an unpinned page for replacement
    // std::mutex latch_;              // to protect shared data structure

    Page *FindUnusedPage(const page_id_t &page_id);
};
}  // namespace huang
