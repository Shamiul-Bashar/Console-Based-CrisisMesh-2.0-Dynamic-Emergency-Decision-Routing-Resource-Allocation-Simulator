#pragma once

#include <cstddef>

namespace crisismesh {

// DSA Algorithm: Manual Merge Sort using divide, recursive sort, and merge.
class MergeSort {
private:
    template <typename T, typename Before>
    // DSA operation: merge two sorted halves.
    static void merge(T* data, T* temp, int left, int mid, int right, Before before) {
        int i = left, j = mid + 1, k = left;
        while (i <= mid && j <= right) {
            if (before(data[i], data[j])) temp[k++] = data[i++];
            else temp[k++] = data[j++];
        }
        while (i <= mid) temp[k++] = data[i++];
        while (j <= right) temp[k++] = data[j++];
        for (int x = left; x <= right; ++x) data[x] = temp[x];
    }

    template <typename T, typename Before>
    // DSA operation: recursively divide the array into smaller ranges.
    static void sortRange(T* data, T* temp, int left, int right, Before before) {
        if (left >= right) return;
        const int mid = left + (right - left) / 2;
        sortRange(data, temp, left, mid, before);
        sortRange(data, temp, mid + 1, right, before);
        merge(data, temp, left, mid, right, before);
    }

public:
    template <typename T, typename Before>
    static void sort(T* data, std::size_t count, Before before) {
        if (count < 2) return;
        T* temp = new T[count];
        sortRange(data, temp, 0, static_cast<int>(count) - 1, before);
        delete[] temp;
    }
};

} // namespace crisismesh
