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
  template <typename... Args>
  SplayRopeNode(Args &&...arg)
      : piece(std::forward<Args>(args)...), left(nullptr), right(nullptr) {}
  void Update() { Piece::UpdateSize(this); }
  size_t size() const { Piece::GetSize(this); }
  size_t GetLeftSize() const { return left ? Piece::GetSize(left) : 0; }
  size_t GetRightSize() const { return right ? Piece::GetSize(right) : 0; }
};

struct DefaultPiece {
  size_t size;
  DefaultPiece() : size(0) {}
  static constexpr size_t kSinglePieceSize = 1;

  static void UpdateSize(SplayRopeNode<DefaultPiece> *node) {
    assert(node);
    node->piece.size = kSinglePieceSize +
                       (node->left ? node->left->piece.size : 0) +
                       (node->right ? node->right->piece.size : 0);
  }

  static size_t GetSize(const SplayRopeNode<DefaultPiece> *node) {
    assert(node);
    return node->piece.size;
  }

  static CompareResult Compare(const size_t index,
                               const SplayRopeNode<DefaultPiece> *node) {
    size_t left_size = node->GetLeftSize();
    static const size_t mid_size = kSinglePieceSize;
    if (index < left_size)
      return {-1, index};
    if (index >= left_size && index < left_size + mid_size)
      return {0, index - left_size};
    return {1, index - (left_size + mid_size)};
  }
};

template <typename Piece = DefaultPiece>
class SplayRope {
public:
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
    Update(root_);
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
      Update(root_);
      return *this;
    }
    assert(i == root_->GetLeftSize());
    Node *const old_left_subtree = root_->left;
    Node *const new_left_subtree = CreateNode(std::forward<Args>(args)...);
    new_left_subtree->left = old_left_subtree;
    Update(new_left_subtree);
    root_->left = new_left_subtree;
    Update(root_);
    root_ = Splay(root_, i);
    return *this;
  }
  virtual ~SplayRope() { Clear(); }

#ifdef EMCC_DEBUG
  size_t CountHeight() const { return CountHeight(root_); }
#endif

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

#ifdef EMCC_DEBUG
  size_t CountHeight(Node *const node) const {
    if (!node)
      return 0;
    return std::max(CountHeight(node->left), CountHeight(node->right)) + 1;
  }
#endif

  Node *RotateLeft(Node *const node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->right == nullptr) {
      return node;
    }
    Node *const parent = node->right;
    node->right = parent->left;
    Update(node);
    parent->left = node;
    Update(parent);
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
    Update(node);
    parent->right = node;
    Update(parent);
    return parent;
  }

  template <typename... Args>
  Node *CreateNode(Args &&...args) {
    Node *n = new Node(std::forward<Args>(args)...);
    Update(n);
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
    return node;
  }

  Node *root_;
};

} // namespace emcc
