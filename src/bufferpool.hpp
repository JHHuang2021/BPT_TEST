#pragma once

#include <utility>

#include "lru_replacer.hpp"
namespace huang {
template <class T, int MAX = 50>
class BufferPool {
   public:
    std::pair<bool, T> Insert(const T& val, int pos) {
        if (replacer.Size() >= MAX) {
            int tmp;
            T ret = replacer.Victim(tmp);
            replacer.Insert(val, pos);
            return std::make_pair(true, ret);
        } else {
            replacer.Insert(val, pos);
            return std::make_pair(false, val);
        }
    }
    std::pair<bool, T> Find(int pos) {
        try {
            return std::make_pair(true, replacer.holder.at(pos));
        } catch (...) {
            return std::make_pair(false, T());
        }
    }
    void Remove(int pos) {
        try {
            replacer.holder.erase(replacer.holder.find(pos));
        } catch (...) {
        }
    }
    T Pop() {
        T ret = replacer.holder.begin()->second;
        replacer.holder.erase(replacer.holder.begin());
        return ret;
    }
    bool Empty() { return replacer.holder.empty(); }

   private:
    Replacer<T, MAX> replacer;
};
}  // namespace huang