#pragma once

#include "dsa/Array.hpp"
#include <stdexcept>

namespace crisismesh {

template <typename T>
class Stack {
private:
    DynamicArray<T> data_;

public:
    void push(const T& value) { data_.pushBack(value); }

    T pop() {
        if (data_.empty()) throw std::runtime_error("Stack underflow");
        T value = data_.back();
        data_.popBack();
        return value;
    }

    const T& top() const {
        if (data_.empty()) throw std::runtime_error("Stack is empty");
        return data_.back();
    }

    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }
    void clear() { data_.clear(); }
};

} // namespace crisismesh
