#ifndef AST_MERKLETREE_H
#define AST_MERKLETREE_H

#include "AST/HashNode.h"
#include "AST/RawTree.h"
#include <list>

namespace diffink {

class MerkleTree {
private:
  std::list<HashNode> NodeStorage;
  HashNode *Root{nullptr};
  std::vector<TSRange> ChangedRanges;

private:
  void build(const RawTree &Tree, const HashNode *Parent, TSTreeCursor &Cursor);

public:
  MerkleTree() noexcept = default;

  void build(const RawTree &Tree);

  HashNode::Set getFullTree() const;

  // static std::pair<HashNode::Set, HashNode::Set>
  // getIdentialAndChangedSubtrees(const MerkleTree &OldTree,
  //                               const MerkleTree &NewTree);
};

} // namespace diffink

#endif // AST_MERKLETREE_H