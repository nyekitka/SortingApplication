#pragma once
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <iostream>
template <class T>
class ArrayIterator;
template <class T>
class ConstArrayIterator;
template <class T>
class DynamicArray {
private:
    T* arr;
    size_t size;
    size_t capacity;
public:
    friend class ArrayIterator<T>;
    friend class ConstArrayIterator<T>;

    using iterator =                ArrayIterator<T>;
    using const_iterator =          ConstArrayIterator<T>;
    using reverse_iterator =        std::reverse_iterator<iterator>;
    using const_reverse_iterator =  std::reverse_iterator<const_iterator>;

    ///////////////////////////////////		constructors	////////////////////////////////////////////

    DynamicArray() {
        arr = nullptr;
        size = 0;
        capacity = 0;
    }
    template <size_t len>
    DynamicArray(const T(&items)[len]) {
        capacity = len * 2;
        size = len;
        arr = new(std::nothrow) T[capacity];
        if (arr == nullptr) {
            capacity = size = 0;
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < len; i++) new (arr + i) T(items[i]);
    }
    DynamicArray(const std::initializer_list<T>& list) {
        size = list.size();
        capacity = 2 * size;
        arr = new(std::nothrow) T[capacity];
        if (arr == nullptr) {
            capacity = size = 0;
            throw std::bad_alloc();
        }
        size_t i = 0;
        for (const auto& iter : list) {
            new (arr + i) T(iter);
            i++;
        }
    }
    DynamicArray(const size_t size, const T elem = T()) {
        this->size = size;
        capacity = 2 * size;
        arr = new(std::nothrow) T[capacity];
        if (arr == nullptr) {
            capacity = this->size = 0;
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < size; i++) {
            if (i) new (arr + i) T(*arr);
            else new (arr) T(std::move(elem));
        }
    }
    DynamicArray(const DynamicArray<T>& other) {
        size = other.size;
        capacity = 2 * size;
        arr = new(std::nothrow) T[capacity];
        if (arr == nullptr) {
            capacity = size = 0;
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < size; i++) new (arr + i) T(other.arr[i]);
    }
    DynamicArray(DynamicArray<T>&& other) noexcept {
        size = other.size;
        capacity = 2 * size;
        arr = other.arr;
        other.arr = nullptr;
        other.size = other.capacity = 0;
    }

    //////////////////////////////////		destructor		//////////////////////////////////////////////

    ~DynamicArray() {
        if (arr != nullptr) delete[] arr;
    }

    /////////////////////////////////		methods		////////////////////////////////////////////////

    iterator begin() noexcept {
        iterator ans(this, 0);
        return ans;
    }
    iterator end() noexcept {
        iterator ans(this, size);
        return ans;
    }
    const_iterator cbegin() const noexcept {
        const_iterator ans(this, 0);
        return ans;
    }
    const_iterator cend() const noexcept {
        const_iterator ans(this, size);
        return ans;
    }
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }
    size_t length() const noexcept {
        return size;
    }
    void resize(const size_t& newsize) {
        if (newsize <= size) size = newsize;
        else if (newsize <= capacity) {
            for (size_t i = size; i < newsize; i++) new (arr + i) T();
            size = newsize;
        }
        else {
            T* newarr = new T[newsize*2];
            capacity = newsize*2;
            for (size_t i = size; i < newsize; i++) new (newarr + i) T();
            for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
            size = newsize;
            delete[] arr;
            arr = newarr;
        }
    }
    void shrink_to_fit() {
        T* newarr = new T[size];
        capacity = size;
        for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
        delete[] arr;
        arr = newarr;
    }
    void reserve(const size_t newcap) {
        if (newcap > capacity) {
            T* newarr = new T[capacity];
            capacity = newcap;
            for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
            delete[] arr;
            arr = newarr;
        }
    }
    void clear() noexcept {
        if (arr != nullptr) delete[] arr;
        capacity = size = 0;
        arr = nullptr;
    }
    void append(const T& value) {
        if (capacity >= size + 1) arr[size++] = value;
        else {
            T* newarr = new T[(size + 1) * 2];
            capacity = (size + 1) * 2;
            newarr[size] = value;
            for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
            delete[] arr;
            arr = newarr;
            size++;
        }
    }
    void append(T&& value) {
        if (capacity >= size + 1) arr[size++] = std::move(value);
        else {
            T* newarr = new T[(size + 1) * 2];
            capacity = (size + 1) * 2;
            newarr[size] = std::move(value);
            for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
            delete[] arr;
            arr = newarr;
            size++;
        }
    }
    void prepend(const T& value) {
        if (capacity >= size + 1) {
            size++;
            for (size_t i = size - 1; i > 0; i--) arr[i] = std::move(arr[i - 1]);
            arr[0] = value;
        }
        else {
            T* newarr = new T[capacity * 2];
            capacity *= 2;
            for (size_t i = size; i > 0; i--) newarr[i] = std::move(arr[i - 1]);
            newarr[0] = value;
            delete[] arr;
            arr = newarr;
            size++;
        }
    }
    void prepend(T&& value) {
        if (capacity >= size + 1) {
            size++;
            for (size_t i = size - 1; i > 0; i--) arr[i] = std::move(arr[i - 1]);
            arr[0] = std::move(value);
        }
        else {
            T* newarr = new T[capacity * 2];
            capacity *= 2;
            for (size_t i = size; i > 0; i--) newarr[i] = std::move(arr[i - 1]);
            newarr[0] = std::move(value);
            delete[] arr;
            arr = newarr;
            size++;
        }
    }
    void insert(const T& value, const size_t& index) {
        if (index >= size) throw std::out_of_range("Array index out of range");
        if (capacity >= size + 1) {
            for (size_t i = size; i > index; i--) arr[i] = std::move(arr[i - 1]);
            arr[index] = value;
            size++;
        }
        else {
            T* newarr = new T[capacity * 2];
            capacity *= 2;
            newarr[index] = value;
            for (size_t i = 0; i < index; i++) newarr[i] = std::move(arr[i]);
            for (size_t i = index + 1; i <= size; i++) newarr[i] = std::move(arr[i - 1]);
            size++;
            delete[] arr;
            arr = newarr;
        }
    }
    void insert(T&& value, const size_t& index) {
        if (index >= size) throw std::out_of_range("Array index out of range");
        if (capacity >= size + 1) {
            for (size_t i = size; i > index; i--) arr[i] = std::move(arr[i - 1]);
            arr[index] = std::move(value);
            size++;
        }
        else {
            T* newarr = new T[capacity * 2];
            capacity *= 2;
            newarr[index] = std::move(value);
            for (size_t i = 0; i < index; i++) newarr[i] = std::move(arr[i]);
            for (size_t i = index + 1; i <= size; i++) newarr[i] = std::move(arr[i - 1]);
            size++;
            delete[] arr;
            arr = newarr;
        }
    }
    void erase(const size_t& index) {
        if (index >= size) throw std::out_of_range("Array index out of range");
        for (size_t i = index + 1; i < size; i++) arr[i - 1] = arr[i];
        size--;
    }
    int find(const T& value) const noexcept {
        for (int i = 0; i < size; i++) {
            if (arr[i] == value) return i;
        }
        return -1;
    }
    DynamicArray<T> subarray(const size_t& begin, const size_t& end) const {
        if (end >= size || begin > end) throw std::out_of_range("Array index out of range");
        DynamicArray<T> ans(end - begin + 1);
        for (size_t i = 0; i <= end - begin; i++) ans[i] = arr[i + begin];
        return ans;
    }

    ////////////////////////////////////	operators		//////////////////////////////////////////////////

    DynamicArray& operator= (const DynamicArray<T>& other) {
        T* newarr;
        if (capacity < other.size) {
            newarr = new T[other.size * 2];
            delete[] arr;
        }
        else {
            newarr = arr;
            capacity = other.size * 2;
        }
        size = other.size;
        for (size_t i = 0; i < size; i++)
            new (newarr + i) T(other.arr[i]);
        arr = newarr;
        return *this;
    }
    DynamicArray& operator= (DynamicArray<T>&& other) noexcept {
        delete[] arr;
        size = other.size;
        capacity = other.capacity;
        arr = other.arr;
        other.arr = nullptr;
        return *this;
    }
    T& operator[] (const size_t& index) {
        if (index >= size) throw std::out_of_range("Array index out of range");
        return arr[index];
    }
    const T& operator[] (const size_t& index) const {
        if (index >= size) throw std::out_of_range("Array index out of range");
        return arr[index];
    }
    bool operator== (const DynamicArray& other) const noexcept {
        if (size != other.size) return false;
        else {
            for (size_t i = 0; i < size; i++) {
                if (arr[i] != other.arr[i]) return false;
            }
        }
        return true;
    }
    bool operator== (const std::initializer_list<T>& other) const noexcept {
        if (size != other.size()) return false;
        else {
            size_t i = 0;
            for (auto it : other) {
                if (it != arr[i]) return false;
                i++;
            }
        }
        return true;
    }
    bool operator!= (const DynamicArray& other) const noexcept {
        return !(*this == other);
    }
    DynamicArray& operator+= (const DynamicArray& other) {
        if (capacity >= size + other.size) {
            for (size_t i = size; i < size + other.size; i++) new (arr + i) T(other.arr[i - size]);
            size += other.size;
        }
        else {
            size_t old_capacity = capacity;
            size + other.size > 2 * capacity ? capacity = size + other.size : capacity *= 2;
            T* newarr = new(std::nothrow) T[old_capacity * 2];
            if (newarr == nullptr) {
                capacity = old_capacity;
                throw std::bad_alloc();
            }
            for (size_t i = size; i < size + other.size; i++) new (newarr + i) T(other.arr[i - size]);
            for (size_t i = 0; i < size; i++) newarr[i] = std::move(arr[i]);
            delete[] arr;
            arr = newarr;
            size += other.size;
        }
        return *this;
    }
    DynamicArray operator+ (const DynamicArray& other) const {
        DynamicArray<T> ans = *this;
        return ans += other;
    }
};

template <class T>
class ArrayIterator {
private:
    DynamicArray<T>* arr;
    size_t index;
    ArrayIterator(DynamicArray<T>* arr, size_t index) : arr(arr), index(index) {}
public:

    using iterator_category =        std::random_access_iterator_tag;
    using value_type =          T;
    using size_type =           size_t;
    using difference_type =     ptrdiff_t;
    using reference =           value_type&;
    using pointer =             T*;

    ArrayIterator() = delete;
    friend class DynamicArray<T>;
    T& operator* () const {
        if (index == arr->size) throw std::out_of_range("Cannot dereference end() of the array");
        else return arr->operator[](index);
    }
    bool operator== (const ArrayIterator& other) const noexcept {
        return arr == other.arr && index == other.index;
    }
    bool operator!= (const ArrayIterator& other) const noexcept {
        return arr != other.arr || index != other.index;
    }
    bool operator> (const ArrayIterator& other) const {
        if (arr != other.arr) throw std::invalid_argument("It's impossible to compare iterators of two different arrays");
        return index > other.index;
    }
    bool operator< (const ArrayIterator& other) const {
        return other > *this;
    }
    bool operator>= (const ArrayIterator& other) const {
        return !(*this < other);
    }
    bool operator<= (const ArrayIterator& other) const {
        return !(*this > other);
    }
    ArrayIterator& operator++ () {
        if (index == arr->size) throw std::out_of_range("Operator ++ isn't applicable to end() of the array");
        ++index;
        return *this;
    }
    ArrayIterator operator++ (int) {
        if (index == arr->size) throw std::out_of_range("Operator ++ isn't applicable to end() of the array");
        ArrayIterator ans = *this;
        ++index;
        return ans;
    }
    ArrayIterator& operator-- () {
        if (index == 0) throw std::out_of_range("Operator -- isn't applicable to begin() of the array");
        --index;
        return *this;
    }
    ArrayIterator operator-- (int) {
        if (index == 0) throw std::out_of_range("Operator -- isn't applicable to begin() of the array");
        ArrayIterator ans = *this;
        --index;
        return ans;
    }
    ArrayIterator& operator+= (int offset) {
        index += offset;
        if (index < 0 || index > arr->size) {
            index -= offset;
            throw std::out_of_range("Going beyond the borders of the array");
        }
        return *this;
    }
    ArrayIterator& operator-= (int offset) {
        index -= offset;
        if (index < 0 || index > arr->size) {
            index += offset;
            throw std::out_of_range("Going beyond the borders of the array");
        }
        return *this;
    }
    ArrayIterator operator+ (int offset) const {
        ArrayIterator ans = *this;
        return ans += offset;
    }
    ArrayIterator operator- (int offset) const {
        ArrayIterator ans = *this;
        return ans -= offset;
    }
    long long operator- (const ArrayIterator& other) const {
        if (arr != other.arr) throw std::invalid_argument("It's impossible to calculate difference between iterators of tow different arrays");
        return static_cast<long long>(index) - static_cast<long long>(other.index);
    }
};
template <class T>
class ConstArrayIterator {
private:
    const DynamicArray<T>* arr;
    size_t index;
    ConstArrayIterator(const DynamicArray<T>* arr, size_t index) : arr(arr), index(index) {}
public:

    using iterator_category =        std::random_access_iterator_tag;
    using value_type =          T;
    using size_type =           size_t;
    using difference_type =     ptrdiff_t;
    using reference =           const value_type&;
    using pointer =             const T*;


    ConstArrayIterator() = delete;
    friend class DynamicArray<T>;
    const T& operator* () const {
        if (index == arr->size) throw std::out_of_range("Cannot dereference end() of the array");
        else return arr->operator[](index);
    }
    bool operator== (const ConstArrayIterator& other) const noexcept {
        return arr == other.arr && index == other.index;
    }
    bool operator!= (const ConstArrayIterator& other) const noexcept {
        return arr != other.arr || index != other.index;
    }
    bool operator> (const ConstArrayIterator& other) const {
        if (arr != other.arr) throw std::invalid_argument("It's impossible to compare iterators of two different arrays");
        return index > other.index;
    }
    bool operator< (const ConstArrayIterator& other) const {
        return other > *this;
    }
    bool operator>= (const ConstArrayIterator& other) const {
        return !(*this < other);
    }
    bool operator<= (const ConstArrayIterator& other) const {
        return !(*this > other);
    }
    ConstArrayIterator& operator++ () {
        if (index == arr->size) throw std::out_of_range("Operator ++ isn't applicable to end() of the array");
        ++index;
        return *this;
    }
    ConstArrayIterator operator++ (int) {
        if (index == arr->size) throw std::out_of_range("Operator ++ isn't applicable to end() of the array");
        ConstArrayIterator ans = *this;
        ++index;
        return ans;
    }
    ConstArrayIterator& operator-- () {
        if (index == 0) throw std::out_of_range("Operator -- isn't applicable to begin() of the array");
        --index;
        return *this;
    }
    ConstArrayIterator operator-- (int) {
        if (index == 0) throw std::out_of_range("Operator -- isn't applicable to begin() of the array");
        ConstArrayIterator ans = *this;
        --index;
        return ans;
    }
    ConstArrayIterator& operator+= (int offset) {
        index += offset;
        if (index < 0 || index > arr->size) {
            index -= offset;
            throw std::out_of_range("Going beyond the borders of the array");
        }
        return *this;
    }
    ConstArrayIterator& operator-= (int offset) {
        index -= offset;
        if (index < 0 || index > arr->size) {
            index += offset;
            throw std::out_of_range("Going beyond the borders of the array");
        }
        return *this;
    }
    ConstArrayIterator operator+ (int offset) const {
        ConstArrayIterator ans = *this;
        return ans += offset;
    }
    ConstArrayIterator operator- (int offset) const {
        ConstArrayIterator ans = *this;
        return ans -= offset;
    }
    long long operator- (const ConstArrayIterator& other) const {
        if (arr != other.arr) throw std::invalid_argument("It's impossible to calculate difference between iterators of tow different arrays");
        return static_cast<long long>(index) - static_cast<long long>(other.index);
    }
};

template <class T>
struct ArrayIterators {
    using iterator =                typename DynamicArray<T>::iterator;
    using const_iterator =          typename DynamicArray<T>::const_iterator;
    using reverse_iterator =        typename DynamicArray<T>::reverse_iterator;
    using const_reverse_iterator =  typename DynamicArray<T>::const_reverse_iterator;
};