// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <cassert>
#include <iostream>
#include <vector>

namespace emcc {

// FIXME: Generalize splay.
template <typename Num>
class PrefixSum {
public:
  PrefixSum() : root_(nullptr) {}

  bool Add(size_t i, Num delta) {
    Node *const node = AtOrNull(i);
    if (!node)
      return false;
    assert(root_ == node);
    node->value += delta;
    node->UpdateSum();
    return true;
  }

  bool Insert(size_t i, Num x) {
    root_ = Splay(root_, i);
    if (root_ == nullptr) {
      root_ = CreateNode(x);
      return true;
    }
    if (i >= root_->size) {
      assert(root_->right == nullptr);
      root_->right = CreateNode(x);
      root_->UpdateSum();
      root_->UpdateSize();
      return true;
    }
    assert(i == root_->GetLeftSize());
    Node *const old_left_subtree = root_->left;
    Node *const new_left_subtree = CreateNode(x);
    new_left_subtree->left = old_left_subtree;
    new_left_subtree->UpdateSize();
    new_left_subtree->UpdateSum();
    root_->left = new_left_subtree;
    root_->UpdateSize();
    root_->UpdateSum();
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
    root_->UpdateSum();
    root_->UpdateSize();
    return true;
  }

  Num At(size_t i) {
    Node *const node = AtOrNull(i);
    if (node == nullptr)
      return Num();
    assert(node == root_);
    return node->value;
  }

  Num GetPrefixSum(size_t i) {
    Node *const node = AtOrNull(i);
    if (!node)
      return Num();
    assert(root_ == node);
    return node->GetLeftSum() + node->value;
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

  ~PrefixSum() { clear(); }

private:
  struct Node {
    Num value;
    Node *left, *right;
    size_t size;
    Num sum;

    void UpdateSum() {
      sum = value + (left ? left->sum : Num()) + (right ? right->sum : Num());
    }
    void UpdateSize() {
      size = 1 + (left ? left->size : 0) + (right ? right->size : 0);
    }
    Num GetLeftSum() const {
      if (left)
        return left->sum;
      return Num();
    }
    size_t GetLeftSize() const {
      if (left)
        return left->size;
      return 0;
    }
    Node(Num value) : value(value), left(nullptr), right(nullptr) {
      UpdateSize();
      UpdateSum();
    }
  };

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
    parent->sum = node->sum;
    parent->size = node->size;
    node->UpdateSum();
    node->UpdateSize();
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
    parent->sum = node->sum;
    parent->size = node->size;
    node->UpdateSum();
    node->UpdateSize();
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
