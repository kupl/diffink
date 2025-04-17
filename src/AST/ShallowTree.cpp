#include "AST/ShallowTree.h"

namespace diffink {

void ShallowTree::build(const HashNode &Root) {
  clear();
  setRoot(pushBack(Root, nullptr));
  buildSubtree(Root, getRoot());
}

void ShallowTree::buildSubtree(const HashNode &ParentHashNode,
                               ShallowTree::Node *Parent) {
  for (const auto &Child : ParentHashNode.getChildren())
    buildSubtree(Child, pushBack(Child, Parent));
}

void ShallowTree::clear() noexcept {
  NodeStore.clear();
  Root = nullptr;
}

ShallowTree::Node *ShallowTree::pushBack(const HashNode &Original,
                                         Node *Parent) {
  auto NewNode =
      &NodeStore.emplace_back(Node{Original, Parent, {}, false, false});
  if (Parent)
    Parent->Children.push_back(NewNode);
  return NewNode;
}

ShallowTree::Node *ShallowTree::insert(const HashNode &Original, Node *Parent,
                                       std::size_t Index) {
  auto NewNode =
      &NodeStore.emplace_back(Node{Original, Parent, {}, false, false});
  if (Parent)
    Parent->Children.insert(Parent->Children.cbegin() + Index, NewNode);
  return NewNode;
}

void ShallowTree::move(Node *Src, std::size_t SrcIndex, Node *Parent,
                       std::size_t Index) {
  Src->Parent->Children.erase(Src->Parent->Children.cbegin() + SrcIndex);
  Parent->Children.insert(Parent->Children.cbegin() + Index, Src);
  Src->Parent = Parent;
}

} // namespace diffink