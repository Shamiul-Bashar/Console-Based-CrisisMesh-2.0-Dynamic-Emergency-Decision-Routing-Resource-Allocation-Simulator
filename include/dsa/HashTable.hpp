#pragma once

#include <cstddef>
#include <string>

namespace crisismesh {

template <typename Value, std::size_t BucketCount = 101>
class HashTable {
private:
    struct Node {
        std::string key;
        Value value;
        Node* next;
        Node(const std::string& k, const Value& v, Node* n) : key(k), value(v), next(n) {}
    };

    Node* buckets_[BucketCount]{};
    std::size_t size_{0};

    static std::size_t hashKey(const std::string& key) {
        unsigned long hash = 5381;
        for (char c : key) hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        return hash % BucketCount;
    }

public:
    HashTable() = default;
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    ~HashTable() { clear(); }

    void put(const std::string& key, const Value& value) {
        const std::size_t bucket = hashKey(key);
        Node* current = buckets_[bucket];
        while (current) {
            if (current->key == key) { current->value = value; return; }
            current = current->next;
        }
        buckets_[bucket] = new Node(key, value, buckets_[bucket]);
        ++size_;
    }

    Value* get(const std::string& key) {
        Node* current = buckets_[hashKey(key)];
        while (current) {
            if (current->key == key) return &current->value;
            current = current->next;
        }
        return nullptr;
    }

    const Value* get(const std::string& key) const {
        Node* current = buckets_[hashKey(key)];
        while (current) {
            if (current->key == key) return &current->value;
            current = current->next;
        }
        return nullptr;
    }

    std::size_t size() const { return size_; }

    void clear() {
        for (std::size_t i = 0; i < BucketCount; ++i) {
            Node* current = buckets_[i];
            while (current) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }
};

} // namespace crisismesh
