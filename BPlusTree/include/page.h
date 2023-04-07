#pragma once

#include <cstring>
#include <iostream>

#include "config.h"

namespace huang {

/**
 * Page is the basic unit of storage within the database system. Page provides a
 * wrapper for actual data pages being held in main memory. Page also contains
 * book-keeping information that is used by the buffer pool manager, e.g. pin
 * count, dirty flag, page id, etc.
 */
class Page {
    friend class BufferPoolManager;
    // There is book-keeping information inside the page that should only be
    // relevant to the buffer pool manager.

   public:
    /** Constructor. Zeros out the page data. */
    Page() { ResetMemory(); }

    /** Default destructor. */
    ~Page(){};

    /** @return the actual data contained within this page */
    char *GetData() { return data_; }

    /** @return the pin count of this page */
    int GetPinCount() { return pin_count_; }

    /** @return true if the page in memory has been modified from the page on
     * disk, false otherwise */
    bool IsDirty() { return is_dirty_; }

    void Clear() {
        ResetMemory();
        pin_count_=0;
        is_dirty_=true;
    }

   private:
    /** Zeroes out the data that is held within the page. */
    inline void ResetMemory() { memset(data_, 0, PAGE_SIZE); }

    /** The actual data that is stored within a page. */
    char data_[PAGE_SIZE]{};

    int pin_count_ = 0;
    /** True if the page is dirty, i.e. it is different from its corresponding
     * page on disk. */
    bool is_dirty_ = false;
};

}  // namespace huang
