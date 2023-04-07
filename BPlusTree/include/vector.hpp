#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>

namespace huang {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
   private:
    int capacity = 20;
    int len = 0;
    T **head = nullptr;

   public:
    /**
     * TODO
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a
        // type. For instance, for an iterator, iterator::value_type is the type
        // that the iterator points to. STL algorithms and containers may use
        // these type_traits (e.g. the following typedef) to work properly. In
        // particular, without the following code,
        // @code{std::sort(iter, iter1);} would not compile.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type:
        // https://blog.csdn.net/u014299153/article/details/72419713 About
        // iterator_category: https://en.cppreference.com/w/cpp/iterator
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

       private:
        pointer *head;
        int pos;

       public:
        int GetPos() { return pos; }

        iterator(pointer *data, int pos) {
            head = data;
            this->pos = pos;
        }
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+(const int &n) const {
            return iterator(this->head, pos + n);
        }
        iterator operator-(const int &n) const {
            return iterator(this->head, pos - n);
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const iterator &rhs) const {
            if (this->head != rhs.head) throw "invalid_iterator";
            return this->pos - rhs.pos;
        }
        iterator &operator+=(const int &n) {
            this->pos += n;
            return *this;
        }
        iterator &operator-=(const int &n) {
            this->pos -= n;
            return *this;
        }
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator it(this->head, this->pos);
            this->pos++;
            return it;
        }
        /**
         * TODO ++iter
         */
        iterator &operator++() {
            this->pos++;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator it(this->head, this->pos);
            this->pos--;
            return it;
        }
        /**
         * TODO --iter
         */
        iterator &operator--() {
            this->pos--;
            return *this;
        }
        /**
         * TODO *it
         */
        T &operator*() const { return *this->head[pos]; }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return this->head == rhs.head && this->pos == rhs.pos;
        }
        bool operator==(const const_iterator &rhs) const {
            return this->head == rhs.head && this->pos == rhs.pos;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;

       private:
        const pointer *head;
        int pos;

       public:
        int GetPos() { return pos; }

        const_iterator(pointer *data, int pos) {
            head = data;
            this->pos = pos;
        }
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator operator+(const int &n) const {
            return (this->head, pos + n);
        }
        const_iterator operator-(const int &n) const {
            return (this->head, pos - n);
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if (this->head != rhs.head) throw "invalid_iterator";
            return this->pos - rhs.pos;
        }
        const_iterator &operator+=(const int &n) {
            this->pos += n;
            return *this;
        }
        const_iterator &operator-=(const int &n) {
            this->pos -= n;
            return *this;
        }
        /**
         * TODO iter++
         */
        const_iterator operator++(int) {
            const_iterator it(this->head, this->pos);
            this->pos++;
            return it;
        }
        /**
         * TODO ++iter
         */
        const_iterator &operator++() {
            this->pos++;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator it(this->head, this->pos);
            this->pos--;
            return it;
        }
        /**
         * TODO --iter
         */
        const_iterator &operator--() {
            this->pos--;
            return *this;
        }
        /**
         * TODO *it
         */
        const T &operator*() const { return *this->head[pos]; }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return this->head == rhs.head && this->pos == rhs.pos;
        }
        bool operator==(const const_iterator &rhs) const {
            return this->head == rhs.head && this->pos == rhs.pos;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    vector() {
        this->len = 0;
        this->head = new T *[capacity];
    }
    vector(const vector &other) {
        this->len = other.len;
        this->head = new T *[this->len];
        for (int i = 0; i < this->len; i++)
            this->head[i] = new T(*other.head[i]);
    }
    /**
     * TODO Destructor
     */
    ~vector() {
        while (!empty()) this->pop_back();
        delete[] head;
    }
    /**
     * TODO Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this == &other) return *this;
        while (!empty()) this->pop_back();
        delete[] head;

        this->len = other.len;
        head = new T *[this->len];
        for (int i = 0; i < this->len; i++)
            this->head[i] = new T(*other.head[i]);
        return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
        if (pos < 0 || pos >= len) throw "index_out_of_bound";
        return *head[pos];
    }
    const T &at(const size_t &pos) const {
        if (pos < 0 || pos >= len) throw "index_out_of_bound";
        return *head[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T &operator[](const size_t &pos) {
        if (pos < 0 || pos >= len) throw "index_out_of_bound";
        return *head[pos];
    }
    const T &operator[](const size_t &pos) const {
        if (pos < 0 || pos >= len) throw "index_out_of_bound";
        return *head[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const {
        if (!len) throw "container_is_empty";
        return *head[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const {
        if (!len) throw "container_is_empty";
        return *head[len - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(this->head, 0); }
    const_iterator cbegin() const { return const_iterator(this->head, 0); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(this->head, this->len); }
    const_iterator cend() const {
        return const_iterator(this->head, this->len);
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const { return !len; }  // to be checked
    /**
     * returns the number of elements
     */
    size_t size() const { return len; }
    /**
     * clears the contents
     */
    void clear() {
        this->len = 0;
        delete[] head;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        push_back(value);
        for (int i = this->len - 2; i >= pos.GetPos(); i--)
            *this->head[i + 1] = *this->head[i];
        *this->head[pos.GetPos()] = value;
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size
     * because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        push_back(value);
        for (int i = this->len - 2; i >= ind; i--)
            *this->head[i + 1] = *this->head[i];
        *this->head[ind] = value;
        return iterator(this->head, ind);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is
     * returned.
     */
    iterator erase(iterator pos) {
        for (int i = pos.GetPos(); i < this->len - 1; i++)
            *this->head[i] = *this->head[i + 1];
        this->pop_back();
        return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind < 0 || ind >= this->len) throw "index_out_of_bound";
        for (int i = ind; i < this->len - 1; i++)
            *this->head[i] = *this->head[i + 1];
        this->pop_back();
        return iterator(this->head, ind);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if (len >= 2 * capacity / 3) {
            T **tmp = new T *[this->len * 2];
            for (int i = 0; i < this->len; i++) tmp[i] = head[i];
            this->capacity *= 2;
            delete[] head;
            head = tmp;
        }
        head[len] = new T(value);
        len++;
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if (!len) throw "container_is_empty";
        len--;
        delete this->head[len];
        if (len <= capacity / 3) {
            T **tmp = new T *[this->len];
            for (int i = 0; i < this->len; i++) tmp[i] = head[i];
            this->capacity = this->len;
            delete[] head;
            head = tmp;
        }
    }
};

}  // namespace huang

#endif
