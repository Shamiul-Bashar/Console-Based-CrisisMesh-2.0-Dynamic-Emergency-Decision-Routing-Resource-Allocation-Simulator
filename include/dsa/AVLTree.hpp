#pragma once

#include <algorithm>
#include <cstddef>
#include <string>

namespace crisismesh {

// DSA: Manual AVL Tree used as a self-balancing binary search tree.
class AVLTree {
private:
    struct Node {
        long long key;
        std::string incidentId;
        int height{1};
        Node* left{nullptr};
        Node* right{nullptr};
        Node(long long k, const std::string& id) : key(k), incidentId(id) {}
    };

    Node* root_{nullptr};
    std::size_t size_{0};

    static int height(Node* n) { return n ? n->height : 0; }
    static int balance(Node* n) { return n ? height(n->left) - height(n->right) : 0; }
    static void update(Node* n) { n->height = 1 + std::max(height(n->left), height(n->right)); }

    // DSA: AVL rotations restore balance after insertion.
    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* t2 = x->right;
        x->right = y;
        y->left = t2;
        update(y); update(x);
        return x;
    }

    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* t2 = y->left;
        y->left = x;
        x->right = t2;
        update(x); update(y);
        return y;
    }

    // DSA operation: BST insertion followed by AVL balance checks.
    Node* insert(Node* node, long long key, const std::string& id, bool& added) {
        if (!node) { added = true; return new Node(key, id); }
        if (key < node->key) node->left = insert(node->left, key, id, added);
        else if (key > node->key) node->right = insert(node->right, key, id, added);
        else { node->incidentId = id; return node; }

        update(node);
        const int b = balance(node);
        if (b > 1 && key < node->left->key) return rotateRight(node);
        if (b < -1 && key > node->right->key) return rotateLeft(node);
        if (b > 1 && key > node->left->key) { node->left = rotateLeft(node->left); return rotateRight(node); }
        if (b < -1 && key < node->right->key) { node->right = rotateRight(node->right); return rotateLeft(node); }
        return node;
    }

    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left); destroy(node->right); delete node;
    }

    // DSA operation: in-order traversal of the AVL Tree.
    template <typename Func>
    static void inorder(Node* node, Func& fn) {
        if (!node) return;
        inorder(node->left, fn);
        fn(node->key, node->incidentId);
        inorder(node->right, fn);
    }

public:
    AVLTree() = default;
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    ~AVLTree() { destroy(root_); }

    void insert(long long key, const std::string& id) {
        bool added = false;
        root_ = insert(root_, key, id, added);
        if (added) ++size_;
    }

    const std::string* find(long long key) const {
        Node* current = root_;
        while (current) {
            if (key == current->key) return &current->incidentId;
            current = key < current->key ? current->left : current->right;
        }
        return nullptr;
    }

    template <typename Func>
    void inorder(Func fn) const { inorder(root_, fn); }

    std::size_t size() const { return size_; }
};

} // namespace crisismesh
