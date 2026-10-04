#pragma once

#include <cstddef>
#include <stdexcept>

namespace crisismesh {

template <typename T>
class Queue {
private:
    T* data_{nullptr};
    std::size_t capacity_{0};
    std::size_t size_{0};
    std::size_t front_{0};
    std::size_t rear_{0};

    void grow() {
        const std::size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
        T* next = new T[newCapacity];
        for (std::size_t i = 0; i < size_; ++i) next[i] = data_[(front_ + i) % capacity_];
        delete[] data_;
        data_ = next;
        capacity_ = newCapacity;
        front_ = 0;
        rear_ = size_;
    }

public:
    Queue() = default;
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;
    ~Queue() { delete[] data_; }

    void enqueue(const T& value) {
        if (size_ == capacity_) grow();
        data_[rear_] = value;
        rear_ = (rear_ + 1) % capacity_;
        ++size_;
    }

    T dequeue() {
        if (size_ == 0) throw std::runtime_error("Queue underflow");
        T value = data_[front_];
        front_ = (front_ + 1) % capacity_;
        --size_;
        return value;
    }

    const T& front() const {
        if (size_ == 0) throw std::runtime_error("Queue is empty");
        return data_[front_];
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    void clear() { size_ = 0; front_ = rear_ = 0; }
};

} // namespace crisismesh
