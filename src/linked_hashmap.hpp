/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <cstddef>
#include <functional>
#include <utility>

namespace huang {
/**
 * In linked_hashmap, iteration ordering is differ from map,
 * which is the order in which keys were inserted into the map.
 * You should maintain a doubly-linked list running through all
 * of its entries to keep the correct iteration order.
 *
 * Note that insertion order is not affected if a key is re-inserted
 * into the map.
 */

template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key> >
class linked_hashmap {
    friend class iterator;
    friend class const_iterator;

   public:
    typedef std::pair<const Key, T> value_type;

   private:
    int cur_mod = 0;
    //to be modified for a more suitable number
    const int MOD[6] = {61, 911, 6907, 51787, 163729, 999983};
    const double half_factor = 0.3;
    const double double_factor = 1.3;
    int __mod = 61;
    struct LinkedNode {
        value_type *value = nullptr;
        LinkedNode *pre = nullptr, *nxt = nullptr;
        LinkedNode *son = nullptr, *fa = nullptr;
        LinkedNode() {}
        LinkedNode(const value_type &val) { value = new value_type(val); }
    };
    LinkedNode *__head = nullptr, *__tail = nullptr;
    LinkedNode **__array = nullptr;
    size_t __size = 0;
    void ReHash(int ind_mod) {
        LinkedNode **tmp_array = new LinkedNode *[MOD[ind_mod]]();
        for (int i = 0; i < MOD[ind_mod]; i++) tmp_array[i] = nullptr;
        int hash;
        for (iterator it = begin(); it != end(); it++) {
            LinkedNode *del = it.__ptr, *fa = del->fa;
            bool empty = false;
            if (!del->fa->fa && !del->son->son) {
                empty = true;
                hash = Hash()(del->value->first) % __mod;
            }
            del->son->fa = del->fa;
            del->fa->son = del->son;
            if (empty) {
                delete fa->son;
                delete fa;
                fa = nullptr;
                __array[hash] = nullptr;
            }

            LinkedNode *ins = it.__ptr;
            ins->fa = nullptr, ins->son = nullptr;
            hash = Hash()(ins->value->first) % MOD[ind_mod];
            if (!tmp_array[hash]) {
                tmp_array[hash] = new LinkedNode();
                tmp_array[hash]->son = new LinkedNode();
            }
            ins->son = tmp_array[hash]->son;
            tmp_array[hash]->son->fa = ins;
            tmp_array[hash]->son = ins;
            ins->fa = tmp_array[hash];
        }
        delete[] __array;
        __array = tmp_array;
        SetMod(ind_mod);
    }

   public:
    void SetMod(int ind_mod) {
        cur_mod = ind_mod;
        __mod = MOD[ind_mod];
    }
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = linked_hashmap.begin(); --it;
     *       or it = linked_hashmap.end(); ++end();
     */
    class const_iterator;
    class iterator {
        friend class linked_hashmap;

       private:
        linked_hashmap<Key, T, Hash, Equal> *__hashmp = nullptr;
        LinkedNode *__ptr = nullptr;

       public:
        using difference_type = std::ptrdiff_t;
        // using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

        iterator() {}
        iterator(linked_hashmap<Key, T, Hash, Equal> *hashmp, LinkedNode *ptr)
            : __ptr(ptr), __hashmp(hashmp) {}
        iterator(const iterator &other) {
            __ptr = other.__ptr;
            __hashmp = other.__hashmp;
        }
        iterator operator++(int) {
            if (__ptr == __hashmp->__tail) throw "index_out_of_bound";
            iterator tmp(__hashmp, __ptr);
            __ptr = __ptr->nxt;
            return tmp;
        }
        iterator &operator++() {
            if (__ptr == __hashmp->__tail) throw "index_out_of_bound";
            __ptr = __ptr->nxt;
            return *this;
        }
        iterator operator--(int) {
            if (__ptr->pre == __hashmp->__head) throw "index_out_of_bound";
            iterator tmp(__hashmp, __ptr);
            __ptr = __ptr->pre;
            return tmp;
        }
        iterator &operator--() {
            if (__ptr->pre == __hashmp->__head) throw "index_out_of_bound";
            __ptr = __ptr->pre;
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory).
         */
        value_type &operator*() const { return *__ptr->value; }
        bool operator==(const iterator &rhs) const {
            return this->__hashmp == rhs.__hashmp && this->__ptr == rhs.__ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return this->__hashmp == rhs.__hashmp && this->__ptr == rhs.__ptr;
        }
        bool operator!=(const iterator &rhs) const {
            return this->__hashmp != rhs.__hashmp || this->__ptr != rhs.__ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return this->__hashmp != rhs.__hashmp || this->__ptr != rhs.__ptr;
        }
        value_type *operator->() const noexcept { return __ptr->value; }
    };

    class const_iterator {
        friend class linked_hashmap<Key, T, Hash, Equal>;

       private:
        const linked_hashmap<Key, T, Hash, Equal> *__hashmp = nullptr;
        const LinkedNode *__ptr = nullptr;

       public:
        const_iterator() {}
        const_iterator(const linked_hashmap<Key, T, Hash, Equal> *hashmp,
                       const LinkedNode *ptr)
            : __hashmp(hashmp), __ptr(ptr) {}
        const_iterator(const const_iterator &other) {
            this->__hashmp = other.__hashmp;
            this->__ptr = other.__ptr;
        }
        const_iterator(const iterator &other) {
            this->__hashmp = other.__hashmp;
            this->__ptr = other.__ptr;
        }
        const_iterator operator++(int) {
            if (__ptr == __hashmp->__tail) throw "index_out_of_bound";
            const_iterator tmp(__hashmp, __ptr);
            __ptr = __ptr->nxt;
            return tmp;
        }
        const_iterator &operator++() {
            if (__ptr == __hashmp->__tail) throw "index_out_of_bound";
            __ptr = __ptr->nxt;
            return *this;
        }
        const_iterator operator--(int) {
            if (__ptr->pre == __hashmp->__head) throw "index_out_of_bound";
            const_iterator tmp(__hashmp, __ptr);
            __ptr = __ptr->pre;
            return tmp;
        }
        const_iterator &operator--() {
            if (__ptr->pre == __hashmp->__head) throw "index_out_of_bound";
            __ptr = __ptr->pre;
            return *this;
        }

        const value_type &operator*() const { return *__ptr->value; }
        bool operator==(const iterator &rhs) const {
            return this->__hashmp == rhs.__hashmp && this->__ptr == rhs.__ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return this->__hashmp == rhs.__hashmp && this->__ptr == rhs.__ptr;
        }
        bool operator!=(const iterator &rhs) const {
            return this->__hashmp != rhs.__hashmp || this->__ptr != rhs.__ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return this->__hashmp != rhs.__hashmp || this->__ptr != rhs.__ptr;
        }
        const value_type *operator->() const noexcept { return __ptr->value; }
    };
    linked_hashmap() {
        __array = new LinkedNode *[__mod]();
        __head = new LinkedNode();
        __tail = new LinkedNode();
        __head->nxt = __tail;
        __tail->pre = __head;
        for (int i = 0; i < __mod; i++) __array[i] = nullptr;
    }
    linked_hashmap(const linked_hashmap &other) {
        __array = new LinkedNode *[__mod]();
        __head = new LinkedNode();
        __tail = new LinkedNode();
        __head->nxt = __tail;
        __tail->pre = __head;
        for (int i = 0; i < __mod; i++) __array[i] = nullptr;
        for (const_iterator it = other.cbegin(); it != other.cend(); it++)
            insert(*it.__ptr->value);
        this->__size = other.__size;
    }
    linked_hashmap &operator=(const linked_hashmap &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); it++)
            insert(*it.__ptr->value);
        this->__size = other.__size;
        return *this;
    }
    ~linked_hashmap() {
        clear();
        delete[] __array;
        delete __head;
        delete __tail;
    }

    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key
     * equivalent to key. If no such element exists, an exception of type
     * `index_out_of_bound'
     */
    T &at(const Key &key) {
        iterator fin = find(key);
        if (fin.__ptr == __tail)
            throw "index_out_of_bound";
        else
            return fin.__ptr->value->second;
    }
    const T &at(const Key &key) const {
        const_iterator fin = find(key);
        if (fin.__ptr == __tail)
            throw "index_out_of_bound";
        else
            return fin.__ptr->value->second;
    }

    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to
     * key, performing an insertion if such key does not already exist.
     */
    T &operator[](const Key &key) {
        iterator fin = find(key);
        if (fin.__ptr == __tail) {
            value_type tmp(key, T());
            std::pair<iterator, bool> rev = insert(tmp);
            return rev.first.__ptr->value->second;
        } else
            return fin.__ptr->value->second;
    }

    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T &operator[](const Key &key) const {
        const_iterator fin = find(key);
        if (fin.__ptr == __tail)
            throw "index_out_of_bound";
        else
            return fin.__ptr->value->second;
    }

    /**
     * return a iterator to the beginning
     */
    iterator begin() { return iterator(this, __head->nxt); }
    const_iterator cbegin() const { return const_iterator(this, __head->nxt); }

    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() { return iterator(this, __tail); }
    const_iterator cend() const { return const_iterator(this, __tail); }

    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const { return !__size; }

    /**
     * returns the number of elements.
     */
    size_t size() const { return __size; }

    /**
     * clears the contents
     */
    void clear() {
        // LinkedNode *del = __head;
        while (!empty()) {
            erase(begin());
        }
        // for (int i = 0; i < __mod; i++) __array[i] = nullptr;
    }

    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the
     * insertion), the second one is true if insert successfully, or false.
     */
    std::pair<iterator, bool> insert(const value_type &value) {
        iterator fin = find(value.first);
        if (fin.__ptr != __tail) return {fin, false};
        LinkedNode *ins = new LinkedNode(value);
        int hash = Hash()(value.first) % __mod;
        if (!__array[hash]) {
            __array[hash] = new LinkedNode();
            __array[hash]->son = new LinkedNode();
        }
        ins->son = __array[hash]->son;
        __array[hash]->son->fa = ins;
        __array[hash]->son = ins;
        ins->fa = __array[hash];

        ins->pre = __tail->pre;
        __tail->pre->nxt = ins;
        ins->nxt = __tail;
        __tail->pre = ins;
        __size++;
        if (__size > double_factor * __mod && cur_mod < 5) ReHash(cur_mod + 1);
        return {iterator(this, ins), true};
    }

    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points
     * an element out of this)
     */
    void erase(iterator pos) {
        if (pos.__ptr == __tail || pos.__hashmp != this)
            throw "invalid_iterator";
        LinkedNode *del = pos.__ptr, *fa = del->fa;
        bool empty = false;
        int hash;
        if (!del->fa->fa && !del->son->son) {
            empty = true;
            hash = Hash()(del->value->first) % __mod;
        }
        del->pre->nxt = del->nxt;
        del->nxt->pre = del->pre;

        del->son->fa = del->fa;
        del->fa->son = del->son;
        if (empty) {
            delete fa->son;
            delete fa;
            fa = nullptr;
            __array[hash] = nullptr;
        }
        delete del->value;
        delete del;
        __size--;
        if (__size < half_factor * __mod && cur_mod > 0) ReHash(cur_mod - 1);
    }

    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     */
    size_t count(const Key &key) const {
        const_iterator fin = find(key);
        if (fin.__ptr != __tail)
            return 1;
        else
            return 0;
    }

    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is
     * returned.
     */
    iterator find(const Key &key) {
        int hash = Hash()(key) % __mod;
        if (__array[hash]) {
            LinkedNode *tmp = __array[hash];
            while (tmp->son->son) {
                if (Equal()(key, tmp->son->value->first))
                    return iterator(this, tmp->son);
                tmp = tmp->son;
            }
        }
        return iterator(this, __tail);
    }
    const_iterator find(const Key &key) const {
        int hash = Hash()(key) % __mod;
        if (__array[hash]) {
            LinkedNode *tmp = __array[hash];
            while (tmp->son->son) {
                if (Equal()(key, tmp->son->value->first))
                    return const_iterator(this, tmp->son);
                tmp = tmp->son;
            }
        }
        return const_iterator(this, __tail);
    }
};

}  // namespace sjtu

#endif
