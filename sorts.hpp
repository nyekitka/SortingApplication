#pragma once
#include "Sequence.h"
#include <set>
#include <ctime>
#include <map>
#include <random>


class ShellSortSequence {
    DynamicArray<size_t>(*seq)(size_t);
public:
    ShellSortSequence() {
        seq = [](size_t size) {
        DynamicArray<size_t> ans;
        while (size /= 2) {
            ans.append(size);
        }
        return ans;
        };
    }
    explicit ShellSortSequence(DynamicArray<size_t>(*seq)(size_t)) {
        this->seq = seq;
    }
    bool operator== (const ShellSortSequence& other) const noexcept {
        return seq == other.seq;
    }
    bool operator!= (const ShellSortSequence& other) const noexcept {
        return seq != other.seq;
    }
    DynamicArray<size_t> operator() (size_t size) const {
        return seq(size);
    }
};
static ShellSortSequence DefaultSequence;
static ShellSortSequence HibbardSequence([](size_t size) {
    size_t max_value = 1, len = 0;
    for (; max_value <= size; max_value <<= 1, ++len) {}
    max_value >>= 1;
    DynamicArray<size_t> arr(len);
    for (size_t i = 0; i < len; ++i) {
        arr[i] = max_value;
        max_value >>= 1;
    }
    return arr;
    });
static ShellSortSequence SedgewickSequence([](size_t size) {
    DynamicArray<size_t> ans(33), powers(33);
    powers[0] = ans[0] = 1;
    size_t i = 1;
    for (; 3 * ans[i - 1] <= size; ++i) {
        if (i == 33) ans.resize(66);
        powers[i] = powers[i - 1] << 1;
        if (i % 2) ans[i] = 8 * powers[i] - 6 * powers[(i + 1) / 2] + 1;
        else ans[i] = 9 * powers[i] - 9 * powers[i / 2] + 1;
    }
    ans.resize(i - 1);
    for (size_t j = 0; j < (i - 1) / 2; ++j) std::swap(ans[j], ans[i - j - 2]);
    return ans;
    });
static ShellSortSequence PrattSequence([](size_t size) {
    DynamicArray<size_t> ans;
    size_t max_value = 1, len = 1, i = 0;
    for (; max_value <= size/2; max_value *= 6, len += 3) {}
    max_value /= 6;
    len -= 3;
    if (max_value * 3 <= size / 2) {
        len += 2;
        ans.resize(len);
        ans[0] = max_value * 3;
        ans[1] = max_value * 2;
        ans[2] = max_value;
        i = 3;
    }
    else if (max_value * 2 <= size / 2) {
        ++len;
        ans.resize(len);
        ans[0] = max_value * 2;
        ans[1] = max_value;
        i = 2;
    }
    else {
        ans.resize(len);
        ans[0] = max_value;
        i = 1;
    }
    for (; max_value /= 6; i += 3) {
        ans[i] = max_value * 3;
        ans[i + 1] = max_value * 2;
        ans[i + 2] = max_value;
    }
    return ans;
    });
static ShellSortSequence FibonacciSequence([](size_t size) {
    DynamicArray<size_t> f(2);
    f[0] = 1;
    f[1] = 2;
    size_t i = 1;
    while (f[i] < size) {
        ++i;
        f.append(f[i - 1] + f[i - 2]);
    }
    f.resize(i);
    for (size_t j = 0; j < i / 2; ++j) std::swap(f[j], f[i - j - 1]);
    return f;
    });


namespace help_functions {
    template <class T, class _Iter>
    _Iter binary_search(const T& value, const _Iter& begin, const _Iter& end, int (*cmp) (T, T)) {
        _Iter middle = begin + (end - begin) / 2;
        int result = cmp(*middle, value);
        if (middle == begin) {
            if (result > 0) return begin;
            else return end;
        }
        if (result > 0) return binary_search(value, begin, middle, cmp);
        else if (result == 0) return middle;
        else return binary_search(value, middle, end, cmp);
    }
	template <class T, class _Iter>
    void merge_sort(_Iter begin, _Iter end, int (*cmp)(T, T)) {
        if (begin == end) return;
        else if (begin == --end) return;
        else ++end;
        int difference = end - begin;
        auto middle = begin + difference / 2;
        help_functions::merge_sort(begin, middle, cmp);
        help_functions::merge_sort(middle, end, cmp);
        auto ptr1 = begin, ptr2 = middle;
        DynamicArray<T> v;
        while (v.length() != difference) {
            if (ptr2 == end || ptr1 != middle && ptr2 != end && cmp(*ptr1, *ptr2) < 0)
                v.append(*(ptr1++));
            else v.append(*(ptr2++));
        }
        auto it2 = v.begin();
        for (auto it1 = begin; it1 != end; ++it1, ++it2) *it1 = *it2;
    }
	template <class T>
    void heapify(DynamicArray<T>& arr, size_t index, int (*cmp) (T, T)) {
        if (index == 0 || index == 1) return;
        if (cmp(arr[index], arr[index / 2]) < 0) {
            std::swap(arr[index], arr[index / 2]);
            help_functions::heapify(arr, index / 2, cmp);
        }
    }
	template <class T>
    void stable_heap(DynamicArray<T>& arr, int (*cmp)(T, T), size_t index = 1) {
        if (arr.length() <= 2) return;
        size_t child1 = 2 * index, child2 = 2 * index + 1;
        T min_value = arr[index];
        if (child1 < arr.length()) {
            min_value = cmp(arr[index], arr[child1]) > 0 ? arr[child1] : arr[index];
            if (child2 < arr.length()) min_value = cmp(min_value, arr[child2]) > 0 ? arr[child2] : min_value;
        }
        if (child1 < arr.length() && min_value == arr[child1]) {
            std::swap(arr[child1], arr[index]);
            help_functions::stable_heap(arr, cmp, child1);
        }
        else if (child2 < arr.length() && min_value == arr[child2]) {
            std::swap(arr[child2], arr[index]);
            help_functions::stable_heap(arr, cmp, child2);
        }
    }
	template <class T, class _Iter>
    void quick_sort(_Iter begin, _Iter end, int (*cmp)(T, T)) {
        long long size = end - begin;
        long long difference = size - 1;
        if (size == 1 || size == 0) return;
        T base = *(begin + rand() % size);
        auto left = begin, right = end - 1;
        do {
            while (cmp(*left, base) < 0 && difference > 0) {
                ++left;
                --difference;
            }
            while (cmp(*right, base) > 0 && difference > 0) {
                --right;
                --difference;
            }
            if (difference > 0) {
                std::swap(*left, *right);
                ++left;
                difference -= 1;
            }
        } while (difference > 0);
        if (left == begin) ++left;
        help_functions::quick_sort(begin, left, cmp);
        help_functions::quick_sort(left, end, cmp);
    }
	template <class T>
    void btsort(DynamicArray<std::pair<T, bool>>& cont, ArrayIterator<std::pair<T, bool>> begin, ArrayIterator<std::pair<T, bool>> end, int (*cmp)(T, T), bool less = true) {
        auto f = [cmp](std::pair<T, bool> val1, std::pair<T, bool> val2) {
            if (val1.second && val2.second) return 0;
            else if (val1.second) return 1;
            else if (val2.second) return -1;
            else return cmp(val1.first, val2.first);
        };
        auto middle = begin + (end - begin) / 2;
        if (middle == begin) return;
        for (auto it1 = begin, it2 = middle; it1 != middle && it2 != end; ++it1, ++it2) {
            if (f(*it1, *it2) > 0 == less) std::swap(*it1, *it2);
        }
        help_functions::btsort(cont, begin, middle, cmp, less);
        help_functions::btsort(cont, middle, end, cmp, less);
    }
	template <class T>
    void make_bitonic(DynamicArray<std::pair<T, bool>>& cont, ArrayIterator<std::pair<T, bool>> begin, ArrayIterator<std::pair<T, bool>> end, int(*cmp)(T, T)) {
        auto middle = begin + (end - begin) / 2;
        if (middle == begin) return;
        help_functions::make_bitonic(cont, begin, middle, cmp);
        help_functions::btsort(cont, begin, middle, cmp);
        help_functions::make_bitonic(cont, middle, end, cmp);
        help_functions::btsort(cont, middle, end, cmp, false);
    }
};


template <class T, class _Iters>
void bubble_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    for (auto i = --sq->end(); i != sq->begin(); --i) {
        for (auto j = sq->begin(); j != i; ++j) {
            auto k = j;
            if (cmp(*j, *(++k)) > 0) std::swap(*k, *j);
        }
    }
}
template <class T, class _Iters>
void shaker_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    typename _Iters::iterator start = sq->begin(), end = --sq->end(), border = --sq->end();
    do {
        for (auto i = start; i != end; ++i) {
            auto k = i + 1;
            if (cmp(*i, *k) > 0) {
                std::swap(*i, *k);
                border = i;
            }
        }
        end = border;
        for (auto i = end; i != start; --i) {
            auto k = i - 1;
            if (cmp(*i, *k) < 0) {
                std::swap(*i, *k);
                border = i;
            }
        }
        start = border;
    } while (end - start > 0);
}
template <class T, class _Iters>
void insertion_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    for (auto i = ++sq->begin(); i != sq->end(); ++i) {
        auto j = i, k = i;
        while (k != sq->begin() && *j < *(--k)) {
            std::swap(*j, *k);
            --j;
        }
    }
}
template <class T, class _Iters>
void selection_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    for (auto i = sq->begin(); i != sq->end(); ++i) {
        auto min = i;
        for (auto j = i; j != sq->end(); ++j) {
            if (cmp(*min, *j) > 0) min = j;
        }
        std::swap(*min, *i);
    }
}
template <class T, class _Iters>
void counting_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    auto f = [cmp](T value1, T value2) {return cmp(value1, value2) < 0; };
    std::map<T, size_t, decltype(f)> counter(f);
    for (auto elem = sq->begin(); elem != sq->end(); ++elem) {
        if (counter.find(*elem) == counter.end()) counter[*elem] = 1;
        else ++counter[*elem];
    }
    auto it = sq->begin();
    for (auto i : counter) {
        while (i.second--) {
            *it = i.first;
            ++it;
        }
    }
}
template <class T, class _Iters>
void bin_insertion_sort(Sequence<T, _Iters>* sq, int (*cmp)(T, T)) {
    for (auto i = ++(sq->begin()); i != sq->end(); ++i) {
        T value = *i;
        auto place = help_functions::binary_search(value, sq->begin(), i, cmp);
        for (auto j = i; j != place; --j) {
            auto k = j - 1;
            *j = *k;
        }
        *place = value;
    }
}
template <class T, class _Iters>
void double_selection_sort (Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    auto end = sq->end();
    for (auto i = sq->begin(); i != sq->end(); ++i) {
        if (end != sq->end()) {
            if (i == end) return;
            else if (--i == end) return;
            else ++i;
        }
        auto min = i;
        auto max = i;
        for (auto j = i; j != end; ++j) {
            if (cmp(*min, *j) > 0) min = j;
            if (cmp(*max, *j) < 0) max = j;
        }
        if (min == --end && max == i) std::swap(*min, *max);
        else if (max == i) {
            std::swap(*max, *end);
            std::swap(*min, *i);
        }
        else {
            std::swap(*min, *i);
            std::swap(*max, *end);
        }
    }
}
template <class T, class _Iters>
void tree_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    auto f = [cmp](T value1, T value2) {return cmp(value1, value2) < 0; };
    std::multiset<T, decltype(f)> s(f);
    for (auto i : *sq) s.insert(i);
    auto it = sq->begin();
    for (auto i : s) *(it++) = i;
}
template <class T, class _Iters>
void merge_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    help_functions::merge_sort(sq->begin(), sq->end(), cmp);
}
template <class T, class _Iters>
void heap_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    size_t size = sq->length();
    if (size == 0) return;
    DynamicArray<T> heap(size + 1);
    auto it = sq->begin();
    for (size_t i = 0; i < size; ++i, ++it) {
        heap[i + 1] = *it;
        help_functions::heapify(heap, i + 1, cmp);
    }
    it = sq->begin();
    for (size_t i = 0; i < size; ++i, ++it) {
        *it = heap[1];
        heap[1] = heap[size - i];
        heap.erase(size - i);
        help_functions::stable_heap(heap, cmp);
    }
}
template <class T, class _Iters>
void quick_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    srand(time(NULL));
    help_functions::quick_sort(sq->begin(), sq->end(), cmp);
}
template<class T, class _Iters>
void shell_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T), ShellSortSequence seq = DefaultSequence) {
    size_t size = sq->length();
    DynamicArray<size_t> shell_seq = seq(size);
    for (auto step : shell_seq) {
        auto start_pos = sq->begin() + step;
        int i_t = step;
        for (auto i = start_pos; i != sq->end(); ++i, ++i_t) {
            T t = *i;
            auto j = i;
            int j_t = i_t;
            for (; j_t >= step; j_t -= step) {
                auto k = j - step;
                if (t < *k) {
                    *j = *k;
                    j = k;
                }
                else break;
            }
            *j = t;
        }
    }
}
template <class T, class _Iters>
void bitonic_sort(Sequence<T, _Iters>* sq, int (*cmp) (T, T)) {
    size_t size = sq->length(), newsize = 1;
    while (newsize < size) newsize <<= 1;
    DynamicArray<std::pair<T, bool> > arr(newsize);
    size_t i = 0;
    for (auto it = sq->begin(); it != sq->end(); ++it, ++i)
        arr[i] = std::pair<T, bool>(*it, false);
    for (; i < newsize; ++i) arr[i] = std::pair<T, bool>(T(), true);
    help_functions::make_bitonic(arr, arr.begin(), arr.end(), cmp);
    help_functions::btsort(arr, arr.begin(), arr.end(), cmp);
    i = 0;
    for (auto it = sq->begin(); it != sq->end(); ++it, ++i) *it = arr[i].first;
}
