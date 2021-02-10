// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <cassert>

namespace emcc {

template <typename Value>
class DynamicArray {
public:
  DynamicArray() : root_(nullptr) {}

  template <typename... Args>
  bool Insert(size_t i, Args &&...args) {
    root_ = Splay(root_, i);
    if (root_ == nullptr) {
      root_ = CreateNode(std::forward<Args>(args)...);
      return true;
    }
    if (i >= root_->size) {
      assert(root_->right == nullptr);
      root_->right = CreateNode(std::forward<Args>(args)...);
      root_->Update();
      return true;
    }
    assert(i == root_->GetLeftSize());
    Node *const old_left_subtree = root_->left;
    Node *const new_left_subtree = CreateNode(std::forward<Args>(args)...);
    new_left_subtree->left = old_left_subtree;
    new_left_subtree->Update();
    root_->left = new_left_subtree;
    root_->Update();
    root_ = Splay(root_, i);
    return true;
  }

  bool Remove(size_t i) {
    Node *const node = AtOrNull(i);
    if (!node)
      return false;
    assert(root_ == node);
    if (root_->right == nullptr) {
      Node *const new_root = root_->left;
      Release(root_);
      root_ = new_root;
      return true;
    }
    Node *right = RotateRightTillEnd(root_->right);
    assert(right);
    assert(right->left == nullptr);
    right->left = root_->left;
    Release(root_);
    root_ = right;
    root_->Update();
    return true;
  }

  Value At(size_t i) {
    Node *const node = AtOrNull(i);
    if (node == nullptr)
      return Value();
    assert(node == root_);
    return node->value;
  }

  size_t size() const {
    if (!root_)
      return 0;
    return root_->size;
  }

  bool empty() const { return size() == 0; }

  void clear() {
    std::vector<Node *> worklist;
    worklist.push_back(root_);
    while (!worklist.empty()) {
      Node *n = worklist.back();
      worklist.pop_back();
      if (n) {
        worklist.emplace_back(n->left);
        worklist.emplace_back(n->right);
      }
      Release(n);
    }
    root_ = nullptr;
  }

  virtual ~DynamicArray() { clear(); }

protected:
  struct Node {
    Value value;
    Node *left, *right;
    size_t size;

    virtual void Update() {
      size = 1 + (left ? left->size : 0) + (right ? right->size : 0);
    }

    size_t GetLeftSize() const {
      if (left)
        return left->size;
      return 0;
    }

    template <typename... Args>
    Node(Args &&...args)
        : value(std::forward<Args>(args)...), left(nullptr), right(nullptr) {
      Update();
    }

    virtual ~Node() {}
  };

private:
  Node *AtOrNull(size_t i) {
    root_ = Splay(root_, i);
    if (!root_)
      return nullptr;
    size_t j = root_->GetLeftSize();
    if (i > j)
      return nullptr;
    assert(i == j);
    return root_;
  }

  template <typename... Args>
  Node *CreateNode(Args &&...args) {
    Node *n = new Node(std::forward<Args>(args)...);
    return n;
  }

  void Release(Node *const node) { delete node; }

  Node *RotateLeft(Node *const node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->right == nullptr) {
      return node;
    }
    Node *const parent = node->right;
    node->right = parent->left;
    parent->left = node;
    node->Update();
    parent->Update();
    return parent;
  };

  Node *RotateRight(Node *const node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->left == nullptr) {
      return node;
    }
    Node *const parent = node->left;
    node->left = parent->right;
    parent->right = node;
    node->Update();
    parent->Update();
    return parent;
  }

  Node *RotateLeftTillEnd(Node *node) {
    if (node == nullptr) {
      return nullptr;
    }
    while (node->right) {
      node = RotateLeft(node);
    }
    return node;
  }

  Node *RotateRightTillEnd(Node *node) {
    if (node == nullptr) {
      return nullptr;
    }
    while (node->left) {
      node = RotateRight(node);
    }
    return node;
  }

  struct CompareResult {
    int order;
  };

  CompareResult Compare(const size_t index, const Node *const node) {
    assert(node);
    if (index < node->GetLeftSize())
      return {-1};
    if (index > node->GetLeftSize())
      return {1};
    return {0};
  }

  Node *Splay(Node *node, const size_t index) {
    if (node == nullptr)
      return nullptr;
    while (true) {
      auto cmp = Compare(index, node);
      if (cmp.order == 0)
        break;
      if (cmp.order < 0) {
        if (node->left == nullptr)
          break;
        cmp = Compare(index, node->left);
        if (cmp.order == 0) {
          node = RotateRight(node);
          break;
        }
        if (cmp.order < 0) {
          if (node->left->left == nullptr) {
            node = RotateRight(node);
            break;
          }
          node = RotateRight(node);
          node = RotateRight(node);
        } else {
          if (node->left->right == nullptr) {
            node = RotateRight(node);
            break;
          }
          node->left = RotateLeft(node->left);
          node = RotateRight(node);
        }
      } else {
        if (node->right == nullptr)
          break;
        cmp = Compare(index, node->right);
        if (cmp.order == 0) {
          node = RotateLeft(node);
          break;
        }
        if (cmp.order < 0) {
          if (node->right->left == nullptr) {
            node = RotateLeft(node);
            break;
          }
          node->right = RotateRight(node->right);
          node = RotateLeft(node);
        } else {
          node = RotateLeft(node);
          node = RotateLeft(node);
        }
      }
    }
    return node;
  }

  Node *root_;
};

} // namespace emcc
