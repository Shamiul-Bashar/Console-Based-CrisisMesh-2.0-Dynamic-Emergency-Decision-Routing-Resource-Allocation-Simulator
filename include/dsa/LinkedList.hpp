#pragma once

#include <cstddef>

namespace crisismesh {

template <typename T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;
        explicit Node(const T& v) : value(v), next(nullptr) {}
    };

    Node* head_{nullptr};
    Node* tail_{nullptr};
    std::size_t size_{0};

public:
    LinkedList() = default;
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    ~LinkedList() { clear(); }

    void pushBack(const T& value) {
        Node* node = new Node(value);
        if (!head_) head_ = tail_ = node;
        else {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    bool popFront(T& out) {
        if (!head_) return false;
        Node* old = head_;
        out = old->value;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;
        delete old;
        --size_;
        return true;
    }

    template <typename Func>
    void forEach(Func fn) const {
        Node* current = head_;
        while (current) {
            fn(current->value);
            current = current->next;
        }
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        Node* current = head_;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

} // namespace crisismesh
