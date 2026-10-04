#pragma once

#include "dsa/Array.hpp"
#include <stdexcept>

namespace crisismesh {

template <typename T, typename LowerPriority>
class MinHeap {
private:
    DynamicArray<T> data_;
    LowerPriority lower_{};

    void swapAt(std::size_t a, std::size_t b) {
        T tmp = data_[a];
        data_[a] = data_[b];
        data_[b] = tmp;
    }

public:
    void push(const T& value) {
        data_.pushBack(value);
        std::size_t i = data_.size() - 1;
        while (i > 0) {
            const std::size_t parent = (i - 1) / 2;
            if (!lower_(data_[i], data_[parent])) break;
            swapAt(i, parent);
            i = parent;
        }
    }

    T pop() {
        if (data_.empty()) throw std::runtime_error("MinHeap is empty");
        T result = data_[0];
        data_[0] = data_.back();
        data_.popBack();
        std::size_t i = 0;
        while (true) {
            const std::size_t left = 2 * i + 1, right = 2 * i + 2;
            std::size_t best = i;
            if (left < data_.size() && lower_(data_[left], data_[best])) best = left;
            if (right < data_.size() && lower_(data_[right], data_[best])) best = right;
            if (best == i) break;
            swapAt(i, best);
            i = best;
        }
        return result;
    }

    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }
};

} // namespace crisismesh
