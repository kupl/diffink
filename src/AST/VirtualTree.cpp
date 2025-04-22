#include "AST/VirtualTree.h"

namespace diffink {

void VirtualTree::makePostOrder(std::vector<Node *> &PostOrder, Node *Iter) {
  if (Iter->Children.empty())
    PostOrder.push_back(Iter);
  else {
    for (auto &Child : Iter->Children)
      makePostOrder(PostOrder, Child);
    PostOrder.push_back(Iter);
  }
}

void VirtualTree::build(const HashNode &Root) {
  clear();
  setRoot(pushBack(Root, nullptr));
  buildSubtree(getRoot());
}

void VirtualTree::buildSubtree(VirtualTree::Node *Root) {
  Root->VirtualHeight = 1;
  for (const auto &Child : Root->Original.getChildren()) {
    auto Temp = pushBack(Child, Root);
    buildSubtree(Temp);
    Root->VirtualHeight =
        std::max(Root->VirtualHeight, Temp->VirtualHeight + 1);
  }
}

void VirtualTree::clear() noexcept {
  NodeStore.clear();
  Root = nullptr;
}

VirtualTree::Node *VirtualTree::pushBack(const HashNode &Original,
                                         Node *Parent) {
  auto NewNode = &NodeStore.emplace_back(Node{Original, Parent});
  if (Parent)
    Parent->Children.push_back(NewNode);
  return NewNode;
}

VirtualTree::Node *VirtualTree::insert(const HashNode &Original, Node *Parent,
                                       std::size_t Index) {
  auto NewNode = &NodeStore.emplace_back(Node{Original, Parent});
  if (Parent)
    Parent->Children.insert(Parent->Children.cbegin() + Index, NewNode);
  return NewNode;
}

void VirtualTree::move(Node *Src, Node *Parent, std::size_t Index) {
  std::size_t SrcIndex = 0;
  for (; SrcIndex != Src->Parent->Children.size(); ++SrcIndex)
    if (Src->Parent->Children[SrcIndex] == Src)
      break;

  Src->Parent->Children.erase(Src->Parent->Children.cbegin() + SrcIndex);
  Parent->Children.insert(Parent->Children.cbegin() + Index, Src);
  Src->Parent = Parent;
}

std::vector<VirtualTree::Node *> VirtualTree::makePostOrder(Node *Root) {
  std::vector<Node *> PostOrder;
  if (Root) {
    PostOrder.reserve(Root->Original.getSize());
    makePostOrder(PostOrder, Root);
  }
  return PostOrder;
}

} // namespace diffink