#ifndef AST_SHALLOWTREE_H
#define AST_SHALLOWTREE_H

#include "AST/HashNode.h"
#include <unordered_map>
#include <unordered_set>

namespace diffink {

class ShallowTree {
public:
  struct Node {
    const HashNode &Original;
    Node *Parent;
    std::vector<Node *> Children;
    bool Marker;
    bool IsVirtual;
  };

private:
  std::list<Node> NodeStore;
  Node *Root{nullptr};

public:
  ShallowTree() noexcept = default;

  ShallowTree(const ShallowTree &Rhs) = delete;

  void build(const HashNode &Root);

  void buildSubtree(const HashNode &ParentHashNode, Node *Parent);

  void clear() noexcept;

  Node *getRoot() const noexcept { return Root; }

  void setRoot(Node *Root) noexcept { this->Root = Root; }

  Node *pushBack(const HashNode &Original, Node *Parent);

  Node *insert(const HashNode &Original, Node *Parent, std::size_t Index);

  static void erase(Node *Parent, std::size_t Index) {
    Parent->Children.erase(Parent->Children.cbegin() + Index);
  }

  static void move(Node *Src, std::size_t SrcIndex, Node *Parent,
                   std::size_t Index);
};

} // namespace diffink

#endif // AST_SHALLOWTREE_H