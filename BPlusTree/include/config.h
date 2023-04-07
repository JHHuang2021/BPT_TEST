#pragma once

// #include <atomic>

namespace huang {

#define INVALID_PAGE_ID -1   // representing an invalid page id
#define INVALID_TXN_ID -1    // representing an invalid txn id
#define HEADER_PAGE_ID 0     // the header page id
#define PAGE_SIZE 25000        // size of a data page in byte
#define BUFFER_POOL_SIZE 10  // size of buffer pool

typedef int page_id_t;  // page id type

}  // namespace huang