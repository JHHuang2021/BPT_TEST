#pragma once

// #include <atomic>
#include <fstream>
// #include <future>  // NOLINT
// #include <mutex>   // NOLINT
#include <iostream>

#include "config.h"

namespace huang {

/**
 * DiskManager takes care of the allocation and deallocation of pages within a
 * database. It performs the reading and writing of pages to and from disk,
 * providing a logical file layer within the context of a database management
 * system.
 */
class DiskManager {
   public:
    /**
     * Creates a new disk manager that writes to the specified database file.
     * @param db_file the file name of the database file to write to
     */
    DiskManager(const std::string &db_file);

    ~DiskManager();

    /**
     * Write a page to the database file.
     * @param page_id id of the page
     * @param page_data raw page data
     */
    void WritePage(page_id_t page_id, const char *page_data);

    /**
     * Read a page from the database file.
     * @param page_id id of the page
     * @param[out] page_data output buffer
     */
    void ReadPage(page_id_t page_id, char *page_data);
    page_id_t AllocatePage();
    void DeallocatePage(page_id_t page_id);

   private:
    int GetFileSize(const std::string &file_name);
    // stream to write db file
    std::fstream db_io_;
    std::string file_name_;

   public:
    page_id_t next_page_id_;
    // atomic
};

}  // namespace huang
