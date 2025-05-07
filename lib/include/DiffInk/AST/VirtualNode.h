#ifndef DIFFINK_AST_VIRTUALNODE_H
#define DIFFINK_AST_VIRTUALNODE_H

#include "DiffInk/AST/HashNode.h"

namespace diffink {

struct VirtualNode {
  const HashNode &Original;
  VirtualNode *Parent{nullptr};
  std::vector<VirtualNode *> Children{};
  bool Marker{false};
  std::size_t VirtualHeight{0};
  XXH128_hash_t VirtualHash;

  void _makePostOrder(std::vector<VirtualNode *> &PostOrder, VirtualNode *Iter);

  void _makeDescendants(std::unordered_set<VirtualNode *> &Descendants,
                        VirtualNode *Iter);

  std::vector<VirtualNode *> makePostOrder();

  std::unordered_set<VirtualNode *> makeDescendants();

  std::size_t findChild(VirtualNode *Child);

  static bool isEqual(const VirtualNode *Left, const VirtualNode *Right) {
    return Left->VirtualHash.high64 == Right->VirtualHash.high64 &&
           Left->VirtualHash.low64 == Right->VirtualHash.low64;
  }

  template <class Function>
  static void traversePostOrder(VirtualNode *Node, Function &&Func);

  template <class Function>
  static void traversePostOrder(VirtualNode *Left, VirtualNode *Right,
                                Function &&Func);
};

} // namespace diffink

#include "DiffInk/AST/VirtualNode.hpp"

#endif // DIFFINK_AST_VIRTUALNODE_H