#ifndef AST_SHALLOWMAP_H
#define AST_SHALLOWMAP_H

#include "AST/ShallowTree.h"

namespace diffink {

struct ShallowMap {
  ShallowTree OldTree;
  ShallowTree NewTree;
  std::unordered_set<std::pair<ShallowTree::Node *, ShallowTree::Node *>>
      Mappings;
  std::unordered_map<ShallowTree::Node *, ShallowTree::Node *> OldToNewMapping;
  std::unordered_map<ShallowTree::Node *, ShallowTree::Node *> NewToOldMapping;

  void insertMapping(ShallowTree::Node *OldNode, ShallowTree::Node *NewNode) {
    Mappings.insert({OldNode, NewNode});
    OldToNewMapping.emplace(OldNode, NewNode);
    NewToOldMapping.emplace(NewNode, OldNode);
  }
};

} // namespace diffink

#endif // AST_SHALLOWMAP_H