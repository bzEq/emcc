// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <cassert>
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
    Piece::UpdateNode(this);
  }
  size_t size() const { return Piece::GetSize(this); }
  size_t left_size() const { return left ? Piece::GetSize(left) : 0; }
  size_t right_size() const { return right ? Piece::GetSize(right) : 0; }
};

struct DefaultPiece {
  size_t size;
  DefaultPiece() : size(0) {}
  static constexpr size_t kSinglePieceSize = 1;

  template <typename T>
  static void UpdateNode(SplayRopeNode<T> *node) {
    assert(node);
    node->piece.size =
        kSinglePieceSize + node->left_size() + node->right_size();
  }

  template <typename T>
  static size_t GetSize(const SplayRopeNode<T> *node) {
    return node ? node->piece.size : 0;
  }

  template <typename T>
  static CompareResult Compare(const size_t index,
                               const SplayRopeNode<T> *node) {
    size_t left_size = node->left_size();
    static const size_t mid_size = kSinglePieceSize;
    if (index < left_size)
      return {-1, index};
    if (index >= left_size && index < left_size + mid_size)
      return {0, index - left_size};
    return {1, index - (left_size + mid_size)};
  }
};

// TODO: Support non-trivial Piece.
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
  size_t size() const { return root_ ? root_->size() : 0; }
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
    Node *right = RotateRightTillEnd(root_->right);
    assert(right);
    assert(right->left == nullptr);
    right->left = root_->left;
    Release(root_);
    root_ = right;
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
    if (i >= root_->size()) {
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
  ~SplayRope() { Clear(); }

#ifdef EMCC_DEBUG
  size_t height() const { return GetHeight(root_); }
#endif

private:
#ifdef EMCC_DEBUG
  size_t GetHeight(Node *const node) const { return node ? node->height : 0; }
#endif

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
    if (node == nullptr)
      return nullptr;
    while (true) {
      auto cmp = Piece::Compare(index, node);
      if (cmp.order == 0)
        break;
      if (cmp.order < 0) {
        if (node->left == nullptr)
          break;
        cmp = Piece::Compare(cmp.relative_index, node->left);
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
        cmp = Piece::Compare(cmp.relative_index, node->right);
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
    return node;
  }

  Node *root_;
};

} // namespace emcc
