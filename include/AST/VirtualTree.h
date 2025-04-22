#ifndef AST_VIRTUALTREE_H
#define AST_VIRTUALTREE_H

#include "AST/HashNode.h"
#include <unordered_map>
#include <unordered_set>

namespace diffink {

class VirtualTree {
public:
  struct Node {
    const HashNode &Original;
    Node *Parent{nullptr};
    std::vector<Node *> Children{};
    bool Marker{false};
    std::size_t VirtualHeight{0};
    xxh::hash_t<HashNode::BitMode> VirtualHash;

    static bool isEqual(const Node *Lhs, const Node *Rhs) {
      return Lhs->VirtualHash.high64 == Rhs->VirtualHash.high64 &&
             Lhs->VirtualHash.low64 == Rhs->VirtualHash.low64;
    }
  };

private:
  std::list<Node> NodeStore;
  Node *Root{nullptr};

private:
  static void makePostOrder(std::vector<Node *> &PostOrder, Node *Iter);

public:
  VirtualTree() noexcept = default;

  VirtualTree(const VirtualTree &Rhs) = delete;

  void build(const HashNode &Root);

  void buildSubtree(Node *Root);

  Node *pushBack(const HashNode &Original, Node *Parent);

  Node *insert(const HashNode &Original, Node *Parent, std::size_t Index);

  void clear() noexcept;

  void setRoot(Node *Root) noexcept { this->Root = Root; }

  Node *getRoot() const noexcept { return Root; }

  static void erase(Node *Parent, std::size_t Index) {
    Parent->Children.erase(Parent->Children.cbegin() + Index);
  }

  static void move(Node *Src, Node *Parent, std::size_t Index);

  static std::vector<Node *> makePostOrder(Node *Root);
};

} // namespace diffink

#endif // AST_VIRTUALTREE_H