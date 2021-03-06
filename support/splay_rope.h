// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <cassert>
#include <iostream>
#include <stddef.h>
#include <utility>
#include <vector>

namespace emcc {

struct CompareResult {
  int order;
  size_t relative_index;
};

template <typename Piece>
struct SplayRopeNode {
  Piece piece;
  SplayRopeNode *left, *right;
  size_t size;
#ifdef EMCC_DEBUG
  size_t height;
#endif
  template <typename... Args>
  SplayRopeNode(Args &&...args)
      : piece(std::forward<Args>(args)...), left(nullptr), right(nullptr) {
#ifdef EMCC_DEBUG
    height = 1;
#endif
  }
  void update() {
#ifdef EMCC_DEBUG
    height = 1 + std::max(left ? left->height : 0, right ? right->height : 0);
#endif
    size = 1 + left_size() + right_size();
    Piece::UpdateNode(this);
  }
  size_t left_size() const { return left ? left->size : 0; }
  size_t right_size() const { return right ? right->size : 0; }
};

struct DefaultPiece {
  DefaultPiece() = default;
  template <typename T>
  static void UpdateNode(SplayRopeNode<T> *node) {}
};

template <typename Piece = DefaultPiece>
class SplayRope {
public:
  static constexpr size_t npos = ~0UL;
  using PieceTy = Piece;
  using Node = SplayRopeNode<Piece>;

  SplayRope() : root_(nullptr) {}
  SplayRope(const SplayRope &) = delete;
  SplayRope(SplayRope &&other) : root_(nullptr) {
    std::swap(root_, other.root_);
  }
  SplayRope &operator=(SplayRope &&other) {
    std::swap(root_, other.root_);
    return *this;
  }
  void clear() { Clear(); }
  size_t size() const { return root_ ? root_->size : 0; }
  bool empty() const { return size() == 0; }
  Piece &At(size_t i) {
    assert(i < size());
    Node *const node = AtOrNull(i);
    assert(node);
    node->update();
    return node->piece;
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
    assert(i == root_->left_size());
    Node *const new_root = Splay(root_->right, 0);
    assert(new_root);
    assert(new_root->left == nullptr);
    new_root->left = root_->left;
    Release(root_);
    root_ = new_root;
    root_->update();
    return true;
  }
  bool Erase(size_t index, size_t len, SplayRope *erased = nullptr);
  SplayRope &Concat(SplayRope &&other);
  SplayRope Split(size_t index);
  template <typename... Args>
  SplayRope &Insert(size_t i, Args &&...args) {
    root_ = Splay(root_, i);
    if (root_ == nullptr) {
      root_ = CreateNode(std::forward<Args>(args)...);
      return *this;
    }
    if (i >= root_->size) {
      assert(root_->right == nullptr);
      root_->right = CreateNode(std::forward<Args>(args)...);
      root_->update();
      return *this;
    }
    assert(i == root_->left_size());
    Node *const old_left_subtree = root_->left;
    Node *const new_left_subtree = CreateNode(std::forward<Args>(args)...);
    new_left_subtree->left = old_left_subtree;
    new_left_subtree->update();
    root_->left = new_left_subtree;
    root_->update();
    root_ = Splay(root_, i);
    return *this;
  }
  template <typename... Args>
  SplayRope &Append(Args &&...args) {
    return Insert(size(), std::forward<Args>(args)...);
  }
  ~SplayRope() { Clear(); }

#ifdef EMCC_DEBUG
  size_t height() const { return GetHeight(root_); }
#endif

private:
#ifdef EMCC_DEBUG
  size_t GetHeight(Node *const node) const { return node ? node->height : 0; }
#endif
  template <typename T>
  static CompareResult Compare(const size_t index,
                               const SplayRopeNode<T> *node) {
    size_t left_size = node->left_size();
    static constexpr size_t mid_size = 1;
    if (index < left_size)
      return {-1, index};
    if (index >= left_size && index < left_size + mid_size)
      return {0, index - left_size};
    return {1, index - (left_size + mid_size)};
  }

  Node *AtOrNull(size_t i) {
    root_ = Splay(root_, i);
    if (!root_)
      return nullptr;
    size_t j = root_->left_size();
    if (i > j)
      return nullptr;
    assert(i == j);
    return root_;
  }

  Node *RotateLeft(Node *const node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->right == nullptr) {
      return node;
    }
    Node *const parent = node->right;
    node->right = parent->left;
    node->update();
    parent->left = node;
    parent->update();
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
    node->update();
    parent->right = node;
    parent->update();
    return parent;
  }

  template <typename... Args>
  Node *CreateNode(Args &&...args) {
    Node *n = new Node(std::forward<Args>(args)...);
    n->update();
    return n;
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

  void Release(Node *const node) { delete node; }

  void Clear() {
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

  Node *Splay(Node *node, const size_t index) {
    return CanonicalSplay(node, index);
  }

  // https://www.link.cs.cmu.edu/link/ftp-site/splaying/top-down-size-splay.c
  Node *CanonicalSplay(Node *node, const size_t index) {
    if (node == nullptr)
      return nullptr;
    size_t key = index;
    std::vector<Node *> update_stack;
    Node N, *l, *r;
    l = r = &N;
    while (true) {
      auto cmp = Compare(key, node);
      key = cmp.relative_index;
      if (cmp.order == 0)
        break;
      if (cmp.order < 0) {
        if (node->left == nullptr)
          break;
        cmp = Compare(key, node->left);
        if (cmp.order < 0) {
          node = RotateRight(node);
          key = cmp.relative_index;
          if (node->left == nullptr)
            break;
        }
        r->left = node;
        r = node;
        update_stack.emplace_back(node);
        node = node->left;
      } else {
        if (node->right == nullptr)
          break;
        cmp = Compare(key, node->right);
        if (cmp.order > 0) {
          node = RotateLeft(node);
          key = cmp.relative_index;
          if (node->right == nullptr)
            break;
        }
        l->right = node;
        l = node;
        update_stack.emplace_back(node);
        node = node->right;
      }
    }
    l->right = node->left;
    l->update();
    r->left = node->right;
    r->update();
    while (!update_stack.empty()) {
      update_stack.back()->update();
      update_stack.pop_back();
    }
    node->left = N.right;
    node->right = N.left;
    node->update();
    assert(node->right == nullptr || node->left_size() == index);
    return node;
  }

  Node *TrickySplay(Node *node, const size_t index) {
    if (node == nullptr)
      return nullptr;
    while (true) {
      auto cmp = Compare(index, node);
      if (cmp.order == 0)
        break;
      if (cmp.order < 0) {
        if (node->left == nullptr)
          break;
        cmp = Compare(cmp.relative_index, node->left);
        if (cmp.order == 0) {
          node = RotateRight(node);
          break;
        }
        if (cmp.order < 0) {
          if (node->left->left == nullptr) {
            node = RotateRight(node);
            break;
          }
          node->left = RotateRight(node->left);
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
        cmp = Compare(cmp.relative_index, node->right);
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
          node->right = RotateLeft(node->right);
          node = RotateLeft(node);
        }
      }
    }
    assert(node->right == nullptr || node->left_size() == index);
    return node;
  }

  Node *root_;
};

} // namespace emcc
