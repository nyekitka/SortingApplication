#pragma once
#include <initializer_list>
#include <stdexcept>
#include <string>


template <class T>
class ListIterator;
template <class T>
class ConstListIterator;

template <class T> class LinkedList {

private:
    class Node {
    public:
        Node* prev;
        T value;
        Node* next;
        Node() {
            prev = next = nullptr;
            value = T();
        }
        bool operator== (const Node other) const {
            return prev == other->prev && next == other->next && value == other->value;
        }
    };
    Node *head;
    Node *tail;
    size_t size;

public:
    friend class ListIterator<T>;
    friend class ConstListIterator<T>;

    using iterator =                ListIterator<T>;
    using const_iterator =          ConstListIterator<T>;
    using reverse_iterator =        std::reverse_iterator<iterator>;
    using const_reverse_iterator =  std::reverse_iterator<const_iterator>;

    ///////////////////////////////////		constructors	////////////////////////////////////////////


    LinkedList() {
        size = 0;
        head = tail = nullptr;
    }
    template <size_t len>
    LinkedList(const T(&items)[len]) {
        size = len;
        if (size == 0) head = tail = nullptr;
        else {
            head = new(std::nothrow) Node;
            if (head == nullptr) {
                size = 0;
                throw std::bad_alloc();
            }
            head->prev = nullptr;
            Node* t = head;
            for (size_t i = 0; i < size; i++) {
                t->value = items[i];
                if (i != size - 1) {
                    t->next = new(std::nothrow) Node;
                    if (t->next == nullptr) {
                        size = i + 1;
                        clear();
                        throw std::bad_alloc();
                    }
                    t->next->prev = t;
                    t = t->next;
                }
                else {
                    t->next = nullptr;
                    tail = t;
                }
            }
        }
    }
    LinkedList(const std::initializer_list<T>& items) {
        if (items.size() == 0) head = tail = nullptr;
        else {
            size = 0;
            head = new(std::nothrow) Node;
            if (head == nullptr) throw std::bad_alloc();
            head->prev = nullptr;
            Node* t = head;
            for (const auto& item : items) {
                ++size;
                t->value = item;
                t->next = new(std::nothrow) Node;
                if (t->next == nullptr) {
                    clear();
                    throw std::bad_alloc();
                }
                t->next->prev = t;
                t = t->next;
            }
            t = t->prev;
            delete t->next;
            t->next = nullptr;
            tail = t;
        }
    }
    LinkedList(const LinkedList<T>& other) {
        size = other.size;
        Node* otherptr = other.head, *ptr = new Node;
        ptr->prev = nullptr;
        if (size) head = ptr;
        for (size_t i = 0; i < size; i++) {
            ptr->value = otherptr->value;
            ptr->next = new(std::nothrow) Node;
            if (ptr->next == nullptr) {
                size = i + 1;
                clear();
                throw std::bad_alloc();
            }
            ptr->next->prev = ptr;
            ptr = ptr->next;
            otherptr = otherptr->next;
        }
        if (size) {
            ptr = ptr->prev;
            delete ptr->next;
            ptr->next = nullptr;
            tail = ptr;
        }
        else {
            delete ptr;
            head = nullptr;
            tail = nullptr;
        }
    }
    LinkedList(LinkedList<T>&& other) noexcept {
        size = other.size;
        head = other.head;
        tail = other.tail;
        other.head = other.tail = nullptr;
        other.size = 0;
    }

    //////////////////////////////////		destructor		//////////////////////////////////////////////

    ~LinkedList() {
        if (size > 1) {
            Node* ptr = head->next;
            while (ptr) {
                delete ptr->prev;
                ptr = ptr->next;
            }
        }
        if (size) delete tail;
    }

    /////////////////////////////////		methods		////////////////////////////////////////////////

    size_t length() const noexcept {
        return size;
    }
    iterator begin() noexcept {
        return iterator(head, false);
    }
    iterator end() noexcept {
        return iterator(tail, true);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(head, false);
    }
    const_iterator cend() const noexcept {
        return const_iterator(tail, true);
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
    T& get_first() const {
        if (size) return &(head->value);
        throw std::out_of_range("List index out of range");
    }
    T& get_last() const {
        if (size) return &(tail->value);
        throw std::out_of_range("List index out of range");
    }
    LinkedList<T> sublist(const size_t& begin, const size_t& end) const {
        if (begin > end || end >= size) throw std::out_of_range("List index out of range");
        LinkedList<T> ans;

        size_t head_ind = 0, tail_ind = size - 1;
        Node* newhead = head, * newtail = tail;
        while (head_ind < begin || tail_ind > end) {
            if (head_ind < begin) {
                newhead = newhead->next;
                head_ind++;
            }
            if (tail_ind > end) {
                newtail = newtail->prev;
                tail_ind--;
            }
        }
        if (begin == end) {
            ans.head = new(std::nothrow) Node;
            if (ans.head == nullptr) throw std::bad_alloc();
            ans.head->value = newhead->value;
            ans.head->prev = nullptr;
            ans.head->next = nullptr;
            ans.tail = ans.head;
            ans.size = 1;
            return ans;
        }
        ans.head = new(std::nothrow) Node;
        if (ans.head == nullptr) throw std::bad_alloc();
        Node* ptr = newhead, *ansptr = ans.head;
        ans.head->prev = nullptr;
        ans.head->value = newhead->value;
        do {
            ptr = ptr->next;
            ++ans.size;
            ansptr->next = new(std::nothrow) Node;
            if (ansptr->next == nullptr) {
                ans.tail = ansptr;
                ans.clear();
                throw std::out_of_range("List index out of range");
            }
            ansptr->next->prev = ansptr;
            ansptr->next->value = ptr->value;
            ansptr = ansptr->next;

        } while (ptr != newtail);
        ansptr->next = nullptr;
        ans.tail = ansptr;
        return ans;
    }
    int find(const T& value) const noexcept {
        Node* ptr1 = head, * ptr2 = tail;
        if (size == 0) return -1;
        for (size_t i = 0; i <= size / 2; i++) {
            if (ptr1->value == value) return i;
            if (ptr2->value == value) return size - 1 - i;
            ptr1 = ptr1->next;
            ptr2 = ptr2->prev;
        }
        return -1;
    }
    void clear() noexcept {
        if (size > 1) {
            Node* ptr = head->next;
            while (ptr) {
                delete ptr->prev;
                ptr = ptr->next;
            }
        }
        if (size) delete tail;
        size = 0;
        head = tail = nullptr;
    }
    void insert(const T& value, const size_t& index) {
        if (index == 0) this->prepend(value);
        else if (index == size) this->append(value);
        else if (index > size) throw std::out_of_range("List index out of range");
        Node* ptr = head, * element = new Node;
        if (index <= size / 2) {
            for (size_t i = 0; i < index; i++) ptr = ptr->next;
        }
        else {
            ptr = tail;
            for (size_t i = size - 1; i > index; i--) ptr = ptr->prev;
        }
        element->value = value;
        element->prev = ptr->prev;
        element->next = ptr;
        ptr->prev->next = element;
        ptr->prev = element;
        size++;
    }
    void insert(T&& value, const size_t& index) {
        if (index == 0) this->prepend(std::move(value));
        else if (index == size) this->append(std::move(value));
        else if (index > size) throw std::out_of_range("List index out of range");
        Node* ptr = head, * element = new Node;
        if (index <= size / 2) {
            for (size_t i = 0; i < index; i++) ptr = ptr->next;
        }
        else {
            ptr = tail;
            for (size_t i = size - 1; i > index; i--) ptr = ptr->prev;
        }
        element->value = std::move(value);
        element->prev = ptr->prev;
        element->next = ptr;
        ptr->prev->next = element;
        ptr->prev = element;
        size++;
    }
    void append(const T& value) {
        if (size == 0) {
            head = new Node;
            tail = head;
            head->value = value;
            size++;
            return;
        }
        tail->next = new Node;
        size++;
        tail->next->prev = tail;
        tail = tail->next;
        tail->next = nullptr;
        tail->value = value;
    }
    void append(T&& value) {
        if (size == 0) {
            head = new Node;
            tail = head;
            head->value = value;
            size++;
            return;
        }
        tail->next = new Node;
        size++;
        tail->next->prev = tail;
        tail = tail->next;
        tail->next = nullptr;
        tail->value = std::move(value);
    }
    void prepend(const T& value) {
        if (size == 0) {
            head = new Node;
            tail = head;
            size++;
            return;
        }
        head->prev = new Node;
        size++;
        head->prev->next = head;
        head = head->prev;
        head->prev = nullptr;
        head->value = value;
    }
    void prepend(T&& value) {
        if (size == 0) {
            head = new Node;
            tail = head;
            size++;
            return;
        }
        head->prev = new Node;
        size++;
        head->prev->next = head;
        head = head->prev;
        head->prev = nullptr;
        head->value = std::move(value);
    }
    void pop_back() {
        if (size == 0) throw std::out_of_range("List index out of range");
        if (size == 1) {
            delete head;
            head = tail = nullptr;
        }
        else {
            tail = tail->prev;
            delete tail->next;
            tail->next = nullptr;
        }
        size--;
    }
    void pop_front() {
        if (size == 0) throw std::out_of_range("List index out of range");
        if (size == 1) {
            delete head;
            head = tail = nullptr;
        }
        else {
            head = head->next;
            delete head->prev;
            head->prev = nullptr;
        }
        size--;
    }
    void erase(const size_t& index) {
        if (index >= size) throw std::out_of_range("List index out of range");
        else if (index == 0) pop_front();
        else if (index == size - 1) pop_back();
        else {
            Node* ptr;
            if (index > size / 2) {
                ptr = tail;
                for (size_t i = size - 1; i != index; i--) ptr = ptr->prev;
            }
            else {
                ptr = head;
                for (size_t i = 0; i != index; i++) ptr = ptr->next;
            }
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
            size--;
            delete ptr;
        }
    }

    ////////////////////////////////////	operators		//////////////////////////////////////////////////

    bool operator== (const LinkedList<T> other) const noexcept {
        Node* ptr = head, *otherptr = other.head;
        while (ptr != nullptr) {
            if (otherptr == nullptr) return false;
            if (ptr->value != otherptr->value) return false;
            ptr = ptr->next;
            otherptr = otherptr->next;
        }
        if (otherptr != nullptr) return false;
        else return true;
    }
    bool operator!= (const LinkedList<T> other) const noexcept {
        return !(*this == other);
    }
    LinkedList<T>& operator= (const LinkedList<T>& other) {
        Node* ptr = head, * otherptr = other.head;
        if (size >= other.size) {
            while (otherptr != nullptr) {
                ptr->value = otherptr->value;
                ptr = ptr->next;
                otherptr = otherptr->next;
            }
            if (ptr != nullptr) {
                while (tail != ptr) {
                    tail = tail->prev;
                    delete tail->next;
                }
                tail = tail->prev;
                delete tail->next;
                tail->next = nullptr;
            }
        }
        else {
            size_t old_size = size;
            if (head == nullptr) {
                tail = head = new Node;
                head->prev = nullptr;
                ++size;
            }
            while (size <= other.size) {
                tail->next = new(std::nothrow) Node;
                if (tail->next == nullptr) {
                    while (size > old_size && tail->prev) {
                        tail = tail->prev;
                        delete tail->next;
                        --size;
                    }
                    if (size == 1 && old_size = 0) {
                        delete tail;
                        head = tail = nullptr;
                        --size;
                    }
                    throw std::bad_alloc();
                }
                tail->next->prev = tail;
                tail = tail->next;
                ++size;
            }
            while (ptr != nullptr) {
                ptr->value = otherptr->value;
                ptr = ptr->next;
                otherptr = otherptr->next;
            }
        }
        return *this;
    }
    LinkedList<T>& operator= (LinkedList<T>&& other) {
        if (head != nullptr) {
            head = head->next;
            while (head != tail) {
                delete head->prev;
                head = head->next;
            }
            delete tail;
        }
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
        return *this;
    }
    T& operator[] (const size_t& index) {
        if (index >= size) throw std::out_of_range("List index out of range");
        if (index > size / 2) {
            Node* ptr = tail;
            for (size_t i = size - 1; i > index; i--) ptr = ptr->prev;
            return ptr->value;
        }
        else {
            Node* ptr = head;
            for (size_t i = 0; i < index; i++) ptr = ptr->next;
            return ptr->value;
        }

    }
    const T& operator[] (const size_t& index) const {
        if(index >= size) throw std::out_of_range("List index out of range");
        if (index > size / 2) {
            Node* ptr = tail;
            for (size_t i = size - 1; i > index; i--) ptr = ptr->prev;
            return ptr->value;
        }
        else {
            Node* ptr = head;
            for (size_t i = 0; i < index; i++) ptr = ptr->next;
            return ptr->value;
        }
    }
    LinkedList<T>& operator+= (const LinkedList<T>& other) {
        Node* ptr = tail,*otherptr = other.head;
        size_t old_size = size;
        if (other.size == 0) return *this;
        if (ptr == nullptr) {
            ptr = new Node;
            ptr->value = otherptr->value;
            head = tail = ptr;
            otherptr = otherptr->next;
            ++size;
        }
        while (otherptr) {
            ptr->next = new(std::nothrow) Node;
            if (ptr->next == nullptr) {
                while (size > old_size && ptr->prev) {
                    ptr = ptr->prev;
                    delete ptr->next;
                    --size;
                }
                if (old_size == 0) {
                    delete ptr;
                    head = tail = nullptr;
                    --size;
                }
                throw std::bad_alloc();
            }
            ptr->next->prev = ptr;
            ptr->next->value = otherptr->value;
            ptr = ptr->next;
            otherptr = otherptr->next;
            ++size;
        }
        ptr->next = nullptr;
        tail = ptr;
        return *this;
    }
    LinkedList<T> operator+ (const LinkedList<T>& other) const {
        LinkedList<T> ans = *this;
        return ans += other;
    }
};

template <class T>
class ListIterator {
private:
    typename LinkedList<T>::Node* node;
    bool isend;
    ListIterator(typename LinkedList<T>::Node* node, bool isend) {
        this->node = node;
        this->isend = isend;
    }
public:

    using iterator_category =        std::random_access_iterator_tag;
    using value_type =          T;
    using size_type =           size_t;
    using difference_type =     ptrdiff_t;
    using reference =           T&;
    using pointer =             T*;

    friend class LinkedList<T>;
    ListIterator() = delete;
    T& operator* () const {
        if (isend) throw std::out_of_range("Cannot dereference end() of the list");
        return node->value;
    }
    bool operator== (const ListIterator& other) const noexcept {
        return node == other.node && isend == other.isend;
    }
    bool operator!= (const ListIterator& other) const noexcept {
        return !(*this == other);
    }
    ListIterator& operator++ () {
        if (isend) throw std::out_of_range("Operator ++ isn't applicable to the end() of the list");
        if (node->next) node = node->next;
        else isend = true;
        return *this;
    }
    ListIterator operator++ (int) {
        if (isend) throw std::out_of_range("Operator ++ isn't applicable to the end() of the list");
        ListIterator ans = *this;
        ++(*this);
        return ans;
    }
    ListIterator& operator-- () {
        if (isend) isend = false;
        else if (node->prev) node = node->prev;
        else throw std::out_of_range("Operator -- isn't applicable to the begin() oh the list");
        return *this;
    }
    ListIterator operator-- (int) {
        ListIterator ans = *this;
        --(*this);
        return ans;
    }
    ListIterator& operator+= (int offset) {
        typename LinkedList<T>::Node* old_node = node;
        if (offset > 0) {
            if (isend) throw std::out_of_range("Going beyond the borders of the list");
            while (offset-- && node->next) {
                node = node->next;
            }
            if (offset == 0) isend = true;
            else if (offset > 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        else if (offset < 0) {
            while (offset++ && node->prev) {
                node = node->prev;
            }
            if (offset < 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        return *this;
    }
    ListIterator& operator-= (int offset) {
        auto old_node = node;
        if (offset > 0) {
            if (isend) {
                --offset;
                isend = false;
            }
            while (offset-- && node->prev) {
                node = node->prev;
            }
            if (offset > 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        else if (offset < 0) {
            if (isend) throw std::out_of_range("Going beyond the borders of the list");
            while (offset++ && node->next) {
                node = node->next;
            }
            if (offset == 0) isend = true;
            else if (offset < 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        return *this;
    }
    ListIterator operator+ (int offset) const {
        ListIterator ans = *this;
        return ans += offset;
    }
    ListIterator operator- (int offset) const {
        ListIterator ans = *this;
        return ans -= offset;
    }
    int operator- (const ListIterator& other) const {
        auto parser = node;
        int difference = 0;
        while (parser) {
            if (parser == other.node) {
                if (isend != other.isend) ++difference;
                return difference;
            }
            parser = parser->prev;
            ++difference;
        }
        parser = node->next;
        difference = -1;
        while (parser) {
            if (parser == other.node) {
                if (isend != other.isend) --difference;
                return difference;
            }
            parser = parser->next;
            --difference;
        }
        throw std::invalid_argument("It's impossible to calculate difference between iterators of tow different alists");
    }
};
template <class T>
class ConstListIterator {
private:
    const typename LinkedList<T>::Node* node;
    bool isend;
    ConstListIterator(const typename LinkedList<T>::Node* node, bool isend) {
        this->node = node;
        this->isend = isend;
    }
public:

    using iterator_category =        std::random_access_iterator_tag;
    using value_type =          T;
    using size_type =           size_t;
    using difference_type =     ptrdiff_t;
    using reference =           const T&;
    using pointer =             const T*;

    friend class LinkedList<T>;
    ConstListIterator() = delete;
    const T& operator* () const {
        if (isend) throw std::out_of_range("Cannot dereference end() of the list");
        return node->value;
    }
    bool operator== (const ConstListIterator& other) const noexcept {
        return node == other.node && isend == other.isend;
    }
    bool operator!= (const ConstListIterator& other) const noexcept {
        return !(*this == other);
    }
    ConstListIterator& operator++ () {
        if (isend) throw std::out_of_range("Operator ++ isn't applicable to the end() of the list");
        if (node->next) node = node->next;
        else isend = true;
        return *this;
    }
    ConstListIterator operator++ (int) {
        if (isend) throw std::out_of_range("Operator ++ isn't applicable to the end() of the list");
        ConstListIterator ans = *this;
        ++(*this);
        return ans;
    }
    ConstListIterator& operator-- () {
        if (isend) isend = false;
        else if (node->prev) node = node->prev;
        else throw std::out_of_range("Operator -- isn't applicable to the begin() oh the list");
        return *this;
    }
    ConstListIterator operator-- (int) {
        ConstListIterator ans = *this;
        --(*this);
        return ans;
    }
    ConstListIterator& operator+= (int offset) {
        typename LinkedList<T>::Node* old_node = node;
        if (offset > 0) {
            if (isend) throw std::out_of_range("Going beyond the borders of the list");
            while (offset-- && node->next) {
                node = node->next;
            }
            if (offset == 0) isend = true;
            else if (offset > 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        else if (offset < 0) {
            while (offset++ && node->prev) {
                node = node->prev;
            }
            if (offset < 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        return *this;
    }
    ConstListIterator& operator-= (int offset) {
        auto old_node = node;
        if (offset > 0) {
            if (isend) {
                --offset;
                isend = false;
            }
            while (offset-- && node->prev) {
                node = node->prev;
            }
            if (offset > 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        else if (offset < 0) {
            if (isend) throw std::out_of_range("Going beyond the borders of the list");
            while (offset++ && node->next) {
                node = node->next;
            }
            if (offset == 0) isend = true;
            else if (offset < 0) {
                node = old_node;
                throw std::out_of_range("Going beyond the borders of the list");
            }
        }
        return *this;
    }
    ConstListIterator operator+ (int offset) const {
        ConstListIterator ans = *this;
        return ans += offset;
    }
    ConstListIterator operator- (int offset) const {
        ConstListIterator ans = *this;
        return ans -= offset;
    }
    int operator- (const ConstListIterator& other) const {
        auto parser = node;
        int difference = 0;
        while (parser) {
            if (parser == other.node) {
                if (isend != other.isend) ++difference;
                return difference;
            }
            parser = parser->prev;
            ++difference;
        }
        parser = node->next;
        difference = -1;
        while (parser) {
            if (parser == other.node) {
                if (isend != other.isend) --difference;
                return difference;
            }
            parser = parser->next;
            --difference;
        }
        throw std::invalid_argument("It's impossible to calculate difference between iterators of tow different alists");
    }
};

template <class T>
struct ListIterators {
    using iterator =                typename LinkedList<T>::iterator;
    using const_iterator =          typename LinkedList<T>::const_iterator;
    using reverse_iterator =        typename LinkedList<T>::reverse_iterator;
    using const_reverse_iterator =  typename LinkedList<T>::const_reverse_iterator;
};