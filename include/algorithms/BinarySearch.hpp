#pragma once

#include <cstddef>
#include <string>

namespace crisismesh {

// DSA Algorithm: Binary Search on sorted IDs.
class BinarySearch {
public:
    template <typename T, std::size_t Capacity>
    static int findById(const T (&items)[Capacity], std::size_t count, const std::string& id) {
        int left = 0;
        int right = static_cast<int>(count) - 1;
        while (left <= right) {
            const int mid = left + (right - left) / 2;
            if (items[mid].id == id) return mid;
            if (items[mid].id < id) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
};

} // namespace crisismesh
