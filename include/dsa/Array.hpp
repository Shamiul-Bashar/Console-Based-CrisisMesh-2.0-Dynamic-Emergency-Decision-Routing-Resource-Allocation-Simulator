#pragma once

#include <cstddef>
#include <stdexcept>

namespace crisismesh {

// DSA: Manual fixed-capacity Array using raw C++ array storage.
template <typename T, std::size_t Capacity>
class StaticArray {
private:
    T data_[Capacity]{};
    std::size_t size_{0};

public:
    bool pushBack(const T& value) {
        if (size_ >= Capacity) return false;
        data_[size_++] = value;
        return true;
    }

    bool popBack() {
        if (size_ == 0) return false;
        --size_;
        return true;
    }

    T& operator[](std::size_t index) {
        if (index >= size_) throw std::out_of_range("StaticArray index out of range");
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) throw std::out_of_range("StaticArray index out of range");
        return data_[index];
    }

    T* data() { return data_; }
    const T* data() const { return data_; }
    std::size_t size() const { return size_; }
    constexpr std::size_t capacity() const { return Capacity; }
    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == Capacity; }
    void clear() { size_ = 0; }
};

// DSA: Manual Dynamic Array using raw pointers and automatic capacity growth.
template <typename T>
class DynamicArray {
private:
    T* data_{nullptr};
    std::size_t size_{0};
    std::size_t capacity_{0};

    // DSA operation: resize the Dynamic Array by doubling its capacity.
    void grow() {
        const std::size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
        T* next = new T[newCapacity];
        for (std::size_t i = 0; i < size_; ++i) next[i] = data_[i];
        delete[] data_;
        data_ = next;
        capacity_ = newCapacity;
    }

public:
    DynamicArray() = default;

    DynamicArray(const DynamicArray& other) {
        capacity_ = other.capacity_;
        size_ = other.size_;
        data_ = capacity_ ? new T[capacity_] : nullptr;
        for (std::size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;
        T* next = other.capacity_ ? new T[other.capacity_] : nullptr;
        for (std::size_t i = 0; i < other.size_; ++i) next[i] = other.data_[i];
        delete[] data_;
        data_ = next;
        size_ = other.size_;
        capacity_ = other.capacity_;
        return *this;
    }

    ~DynamicArray() { delete[] data_; }

    void pushBack(const T& value) {
        if (size_ == capacity_) grow();
        data_[size_++] = value;
    }

    void popBack() {
        if (size_ > 0) --size_;
    }

    T& back() {
        if (size_ == 0) throw std::runtime_error("DynamicArray is empty");
        return data_[size_ - 1];
    }

    const T& back() const {
        if (size_ == 0) throw std::runtime_error("DynamicArray is empty");
        return data_[size_ - 1];
    }

    T& operator[](std::size_t index) {
        if (index >= size_) throw std::out_of_range("DynamicArray index out of range");
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) throw std::out_of_range("DynamicArray index out of range");
        return data_[index];
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    void clear() { size_ = 0; }
};

} // namespace crisismesh
