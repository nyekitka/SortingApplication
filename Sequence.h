#pragma once
#include "DynamicArray.h"
#include "LinkedList.h"
#include <cstring>
template <class T, class _Iter>
class Sequence;

template <class T, class _Iterators>
class Sequence {
public:

    using iterator =                typename _Iterators::iterator;
    using const_iterator =          typename _Iterators::const_iterator;
    using reverse_iterator =        typename _Iterators::reverse_iterator;
    using const_reverse_iterator =  typename _Iterators::const_reverse_iterator;

    virtual iterator begin() noexcept = 0;
    virtual iterator end() noexcept = 0;
    virtual const_iterator cbegin() const noexcept = 0;
    virtual const_iterator cend() const noexcept = 0;
    virtual reverse_iterator rbegin() noexcept = 0;
    virtual reverse_iterator rend() noexcept = 0;
    virtual const_reverse_iterator crbegin() const noexcept = 0;
    virtual const_reverse_iterator crend() const noexcept = 0;
    virtual size_t length() const noexcept = 0;
    virtual T& get_first() noexcept = 0;
    virtual T& get_last() noexcept = 0;
    virtual const T& get_first() const noexcept = 0;
    virtual const T& get_last() const noexcept = 0;
    virtual void append(const T&) = 0;
    virtual void append(T&&) = 0;
    virtual void clear() noexcept = 0;
    virtual void prepend(const T&) = 0;
    virtual void prepend(T&&) = 0;
    virtual void insert(const T&, const size_t&) = 0;
    virtual void insert(T&&, const size_t&) = 0;
    virtual void erase(const size_t&) noexcept = 0;
    virtual int find(const T&) const noexcept = 0;
    virtual T& operator[] (const size_t&) = 0;
    virtual const T& operator[] (const size_t&) const = 0;
    virtual Sequence& operator+= (const Sequence&) = 0;
};

#define DECL_(CLASS, TYPE, type)        																			\
template <class T>																									\
class TYPE##Sequence : public Sequence<T, TYPE##Iterators<T> >{														\
private:																											\
    CLASS<T> arr;																									\
public:																												\
                                                                                                                    \
    using _Mybase =                 Sequence<T, TYPE##Iterators<T> >;                                               \
    using iterator =                typename _Mybase::iterator;                                                     \
    using const_iterator =          typename _Mybase::const_iterator;                                               \
    using reverse_iterator =        typename _Mybase::reverse_iterator;                                             \
    using const_reverse_iterator =  typename _Mybase::const_reverse_iterator;                                       \
                                                                                                                    \
    TYPE##Sequence() = default;																						\
    template <size_t len> TYPE##Sequence(const T(&items)[len]) : arr(items)	{}										\
    TYPE##Sequence(const std::initializer_list<T>& items) : arr(items) {}											\
    size_t length() const noexcept override {																		\
        return arr.length();																						\
    }																												\
    iterator begin() noexcept override {        																    \
        return arr.begin();																					        \
    }																												\
    iterator end() noexcept override {																			    \
        return arr.end();																					        \
    }																												\
    const_iterator cbegin() const noexcept override {                                                               \
        return arr.cbegin();                                                                                        \
    }                                                                                                               \
    const_iterator cend() const noexcept override {                                                                 \
        return arr.cend();                                                                                          \
    }                                                                                                               \
    reverse_iterator rbegin() noexcept override {                                                                   \
        return arr.rbegin();                                                                                        \
    }                                                                                                               \
    reverse_iterator rend() noexcept override {                                                                     \
        return arr.rend();                                                                                          \
    }                                                                                                               \
    const_reverse_iterator crbegin() const noexcept override {                                                      \
        return arr.crbegin();                                                                                       \
    }                                                                                                               \
    const_reverse_iterator crend() const noexcept override {                                                        \
        return arr.crend();                                                                                         \
    }                                                                                                               \
    T& get_first() noexcept override {		    																	\
        return arr[0];																								\
    }																												\
    T& get_last() noexcept override {	    																		\
        return arr[arr.length() - 1];																				\
    }																												\
    const T& get_first() const noexcept override {		    				    									\
        return arr[0];																								\
    }																												\
    const T& get_last() const noexcept override {	    															\
        return arr[arr.length() - 1];																				\
    }																												\
    void clear() noexcept override {																				\
        arr.clear();																								\
    }																												\
    void append(const T& value) override {																			\
        return arr.append(value);																					\
    }																												\
    void append(T&& value) override {																				\
        return arr.append(std::move(value));																		\
    }																												\
    void prepend(const T& value) override {																			\
        return arr.prepend(value);																					\
    }																												\
    void prepend(T&& value) override {																				\
        return arr.prepend(std::move(value));																		\
    }																												\
    int find(const T& value) const noexcept override {																\
        return arr.find(value);																						\
    }																												\
    TYPE##Sequence<T> subseq(const size_t& begin, const size_t& end) const {										\
        TYPE##Sequence<T> ans;																						\
        ans.arr = arr.sub##type(begin, end);																		\
        return ans;																									\
    }																												\
    void insert(const T& value, const size_t& index) override {														\
        arr.insert(value, index);																					\
    }																												\
    void insert(T&& value, const size_t& index) override {															\
        arr.insert(std::move(value), index);																		\
    }																												\
    void erase(const size_t& index) noexcept override {																\
        arr.erase(index);																							\
    }																												\
    bool operator== (const TYPE##Sequence<T>& other) const noexcept {												\
        return arr == other.arr;																					\
    }																												\
    bool operator!= (const TYPE##Sequence<T>& other) const noexcept {												\
        return arr != other.arr;																					\
    }																												\
    T& operator[] (const size_t& index) override {																	\
        return arr[index];																							\
    }																												\
    const T& operator[] (const size_t& index) const override {														\
        return arr[index];																							\
    }																												\
    _Mybase& operator+= (const _Mybase& other) override {		        											\
        arr += static_cast<const TYPE##Sequence<T>&>(other).arr;													\
        return *this;																								\
    }																												\
    TYPE##Sequence<T> operator+ (const TYPE##Sequence<T>& other) const {                                            \
        TYPE##Sequence<T> ans = *this;                                                                              \
        return ans += other;                                                                                        \
    }                                                                                                               \
};
DECL_(DynamicArray, Array, array)
DECL_(LinkedList, List, list)
