#ifndef AST_MERKLETREE_H
#define AST_MERKLETREE_H

#include "AST/HashNode.h"
#include <forward_list>
#include <list>

namespace diffink {

class MerkleTree {
private:
  std::list<HashNode> NodeList;

public:
  MerkleTree() = default;
};

} // namespace diffink

#endif // AST_MERKLETREE_H