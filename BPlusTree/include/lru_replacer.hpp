#pragma once

#include <mutex>  // NOLINT

#include "config.h"
#include "linked_hashmap.hpp"

namespace huang {

/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
template <typename T>
class Replacer {
   public:
    Replacer(){};
    ~Replacer(){};
    void Insert(const T &value) {
        try {
            holder.erase(holder.find(value));
        } catch (...) {
        }
        holder[value] = 1;
        return;
    }
    bool Victim(T &value) {
        if (holder.empty()) return false;
        value = (*holder.begin()).first;
        holder.erase(holder.begin());
        return true;
    };
    void Erase(const T &value) { holder.erase(holder.find(value)); }
    size_t Size() { return holder.size(); }

   private:
    linked_hashmap<T, int> holder;
};

}  // namespace huang
