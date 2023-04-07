#pragma once

#include "linked_hashmap.hpp"

namespace huang {

/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
template <typename T, int MAX = 50>
class Replacer {
   public:
    Replacer(){};
    ~Replacer(){};
    void Insert(const T &value, int pos) {
        try {
            holder.erase(holder.find(pos));
        } catch (...) {
        }
        holder[pos] = value;
        return;
    }
    T Victim(int &pos) {
        pos = (*holder.begin()).first;
        T ret=holder.begin()->second;
        holder.erase(holder.begin());
        return ret;
    };
    size_t Size() { return holder.size(); }

    linked_hashmap<int, T> holder;
};

}  // namespace huang
