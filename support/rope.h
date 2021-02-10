// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <assert.h>
#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace emcc {

template <typename Char,
          template <typename...> typename ContainerType = std::basic_string,
          size_t kMaxPieceSize = 4096>
class Rope {
private:
  using Piece = ContainerType<Char>;

  struct Node {
    size_t size;
    Piece piece;
    Node *left, *right;

    Node() : size(0), left(nullptr), right(nullptr) {}

    Node(Piece &&p)
        : size(p.size()), piece(std::move(p)), left(nullptr), right(nullptr) {}

    Node(Node *l, Node *r, Piece &&p)
        : size(0), piece(std::move(p)), left(l), right(r) {
      UpdateSize();
    }

    void UpdateSize() {
      size = piece.size() + (left ? left->size : 0) + (right ? right->size : 0);
    }
  };

  Node *root_;

  template <typename... Args>
  Node *CreateNode(Args &&...args) {
    Node *n = new Node(std::forward<Args>(args)...);
    return n;
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
    parent->left = node;
    parent->size = node->size;
    node->UpdateSize();
    return parent;
  }

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
    parent->size = node->size;
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

  std::tuple<Node *, Node *> Split(Node *node, const size_t index) {
    if (node == nullptr) {
      return std::make_tuple(nullptr, nullptr);
    }
    node = Splay(node, index);
    auto cmp = Compare(index, node);
    if (cmp.order != 0) {
      return std::make_tuple(node, nullptr);
    }
    Piece left_piece(node->piece.begin(),
                     node->piece.begin() + cmp.relative_index);
    Node *const left = left_piece.empty() ? node->left
                                          : CreateNode(node->left, nullptr,
                                                       std::move(left_piece));
    Piece right_piece(node->piece.begin() + cmp.relative_index,
                      node->piece.end());
    Node *const right =
        right_piece.empty()
            ? node->right
            : CreateNode(nullptr, node->right, std::move(right_piece));
    Release(node);
    return std::make_tuple(left, right);
  }

  Node *Concat(Node *const lhs, Node *const rhs) {
    if (lhs == nullptr) {
      return rhs;
    }
    Node *const l = RotateLeftTillEnd(lhs);
    assert(l->right == nullptr);
    Node *const r = RotateRightTillEnd(rhs);
    if (r != nullptr && l->piece.size() + r->piece.size() <= kMaxPieceSize) {
      assert(r->left == nullptr);
      // Compress the piece.
      l->piece.insert(l->piece.end(), r->piece.begin(), r->piece.end());
      l->right = r->right;
      Release(r);
    } else {
      l->right = r;
    }
    l->UpdateSize();
    return l;
  }

  // ftp://ftp.cs.cmu.edu/usr/ftp/usr/sleator/splaying/top-down-splay.c
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

  struct CompareResult {
    int order;
    size_t relative_index;
  };

  CompareResult Compare(const size_t index, const Node *const node) {
    assert(node);
    const size_t left_subtree_size = node->left ? node->left->size : 0;
    if (index < left_subtree_size) {
      return {-1, index};
    }
    const size_t left_size = left_subtree_size + node->piece.size();
    if (index >= left_size) {
      return {1, index - left_size};
    }
    return {0, index - left_subtree_size};
  }

  void Release(Node *const node) { delete node; }

  Node *Insert(Node *node, const size_t index, Char c) {
    if (node == nullptr) {
      node = CreateNode();
      node->piece.push_back(c);
      node->UpdateSize();
      return node;
    }
    assert(index <= node->size);
    node = Splay(node, index);
    auto cmp = Compare(index, node);
    assert(cmp.order >= 0);
    if (node->piece.size() < kMaxPieceSize) {
      if (cmp.order > 0) {
        assert(cmp.relative_index == 0);
        node->piece.push_back(c);
      } else {
        assert(cmp.relative_index < node->piece.size());
        node->piece.insert(node->piece.begin() + cmp.relative_index, c);
      }
      node->UpdateSize();
      return node;
    }
    if (cmp.order > 0) {
      assert(node->right == nullptr);
      assert(cmp.relative_index == 0);
      node->right = CreateNode();
      node->right->piece.push_back(c);
      node->right->UpdateSize();
      node->UpdateSize();
      return node;
    }
    assert(cmp.order == 0);
    assert(cmp.relative_index < node->piece.size());
    assert(node->piece.size() == kMaxPieceSize);
    Piece right_piece(node->piece.begin() + kMaxPieceSize / 2,
                      node->piece.end());
    Node *right_subtree = CreateNode(std::move(right_piece));
    right_subtree->right = node->right;
    node->piece.resize(kMaxPieceSize / 2);
    node->right = right_subtree;
    if (cmp.relative_index < kMaxPieceSize / 2) {
      node->piece.insert(node->piece.begin() + cmp.relative_index, c);
    } else {
      right_subtree->piece.insert(right_subtree->piece.begin() +
                                      (cmp.relative_index - kMaxPieceSize / 2),
                                  c);
    }
    right_subtree->UpdateSize();
    node->UpdateSize();
    return node;
  }

public:
  Rope() : root_(nullptr) {}

  Rope(const Piece &p) : root_(nullptr) { Append(p); }

  Rope(const Rope &other) = delete;

  Rope(Rope &&other) : root_(nullptr) { std::swap(root_, other.root_); }

  Rope &swap(Rope &&other) {
    std::swap(root_, other.root_);
    return *this;
  }

  Rope &Concat(Rope &&other) {
    root_ = Concat(root_, other.root_);
    other.root_ = nullptr;
    return *this;
  }

  Rope Split(const size_t i) {
    Node *tail = nullptr;
    std::tie(root_, tail) = Split(root_, i);
    Rope r;
    r.root_ = tail;
    return std::move(r);
  }

  Char At(const size_t index) {
    assert(root_);
    assert(index < root_->size);
    root_ = Splay(root_, index);
    auto cmp = Compare(index, root_);
    assert(cmp.order == 0);
    return root_->piece[cmp.relative_index];
  }

  template <typename Iterator>
  size_t Insert(const size_t index, Iterator begin, Iterator end) {
    size_t i = std::min(index, root_ ? root_->size : 0), res = 0;
    for (auto it = begin; it != end; ++it) {
      root_ = Insert(root_, i++, *it);
      ++res;
    }
    return res;
  }

  size_t Insert(const size_t index, const Piece &piece) {
    return Insert(index, piece.begin(), piece.end());
  }

  bool Insert(const size_t index, Char c) {
    root_ = Insert(root_, index, c);
    return true;
  }

  size_t Erase(const size_t index, const size_t len, Rope *erased = nullptr) {
    if (root_ == nullptr || len == 0) {
      return 0;
    }
    Node *from = nullptr;
    std::tie(root_, from) = Split(root_, index);
    if (from == nullptr) {
      return 0;
    }
    Node *to = nullptr, *tail = nullptr;
    std::tie(to, tail) = Split(from, len);
    assert(to);
    size_t num_erased = to->size;
    if (erased) {
      erased->root_ = Concat(erased->root_, to);
    } else {
      Release(to);
    }
    if (tail == nullptr) {
      return num_erased;
    }
    assert(len == num_erased);
    root_ = Concat(root_, tail);
    return num_erased;
  }

  void Append(Char c) {
    if (root_ == nullptr)
      root_ = Insert(root_, 0, c);
    else
      root_ = Insert(root_, root_->size, c);
  }

  template <typename Iterator>
  size_t Append(Iterator begin, Iterator end) {
    size_t count = 0;
    for (auto it = begin; it != end; ++it) {
      Append(*it);
      ++count;
    }
    return count;
  }

  size_t Append(const Piece &piece) {
    return Append(piece.begin(), piece.end());
  }

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
      delete n;
    }
    root_ = nullptr;
  }

  ~Rope() { clear(); }

  size_t size() const {
    if (root_ == nullptr) {
      return 0;
    }
    return root_->size;
  }

  bool empty() const { return size() == 0; }
};

inline std::ostream &operator<<(std::ostream &out,
                                Rope<char, std::basic_string> &rope) {
  for (size_t i = 0; i < rope.size(); ++i)
    out << rope.At(i);
  return out;
}

template <size_t N>
inline bool operator==(Rope<char, std::basic_string, N> &rope,
                       const std::string &s) {
  if (rope.size() != s.size())
    return false;
  for (size_t i = 0; i != rope.size(); ++i)
    if (rope.At(i) != s[i])
      return false;
  return true;
}

template <size_t N>
inline bool operator==(Rope<char, std::basic_string, N> &rope, const char *s) {
  if (rope.size() != ::strlen(s))
    return false;
  for (size_t i = 0; i != rope.size(); ++i)
    if (rope.At(i) != s[i])
      return false;
  return true;
}

} // namespace emcc
