#include "include/disk_manager.h"

namespace huang {
DiskManager::DiskManager(const std::string &db_file) {
    // with file open
    file_name_ = db_file;
    db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
    // directory or file does not exist
    if (!db_io_.is_open()) {
        db_io_.clear();
        // create a new file
        db_io_.open(db_file, std::ios::out);
        // reopen with original mode
        // db_io_.open(db_file, std::ios::binary | std::ios::in |
        // std::ios::out);
    }
    db_io_.close();
}
DiskManager::~DiskManager() { }
void DiskManager::ReadPage(page_id_t page_id, char *page_data) {
    size_t offset = page_id * PAGE_SIZE;
    // set write cursor to offset
    db_io_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    db_io_.seekp(offset);
    db_io_.read(page_data, PAGE_SIZE);
    db_io_.close();
}

void DiskManager::WritePage(page_id_t page_id, const char *page_data) {
    int offset = page_id * PAGE_SIZE;
    // set read cursor to offset
    db_io_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    db_io_.seekp(offset);
    db_io_.write(page_data, PAGE_SIZE);
    db_io_.close();
}

page_id_t DiskManager::AllocatePage() { return next_page_id_++; }

};  // namespace huang