#ifndef AST_MERKLETREE_H
#define AST_MERKLETREE_H

#include "AST/ShallowMap.h"
#include "TreeDiff/EditAction.h"
#include "Utils/NodeComp.h"
#include "Utils/TextDiff.h"
#include <queue>

namespace diffink {

class MerkleTree {
private:
  std::unique_ptr<HashNode> Root;
  std::unique_ptr<TSTree, decltype(&ts_tree_delete)> RawTree;
  std::unordered_set<const HashNode *> StructuralChanges;
  std::unordered_set<const HashNode *> TextualChanges;

private:
  struct Iterators {
    const HashNode &OldHashIter;
    TSTreeCursor &EditedCursor;
    const HashNode &NewHashIter;
    TSTreeCursor &NewCursor;
  };

  void identifyChange(MerkleTree &OldTree, Iterators Iters);

  void clearChanges() noexcept;

public:
  MerkleTree() noexcept : RawTree(nullptr, ts_tree_delete) {}

  void swap(MerkleTree &Rhs) noexcept;

  void clear() noexcept;

  // Return false if the tree has an error node
  bool parse(TSParser &Parser, const SourceCode &Code);

  // Return false if the tree has an error node
  bool parse(TSParser &Parser, MerkleTree &OldTree, const SourceCode &OldCode,
             const SourceCode &Code, const EditSequence &Seq);

  const HashNode &getRoot() const noexcept { return *Root; }

  void copyFullTree(ShallowTree &TreeCopy) const { TreeCopy.build(*Root); }

  static EditScript copyChangedSubtree(ShallowMap &Map,
                                       const MerkleTree &OldTree,
                                       const MerkleTree &NewTree);
};

} // namespace diffink

#endif // AST_MERKLETREE_H