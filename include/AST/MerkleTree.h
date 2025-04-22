#ifndef AST_MERKLETREE_H
#define AST_MERKLETREE_H

#include "TreeDiff/EditAction.h"
#include "Utils/NodeComp.h"
#include "Utils/TextDiff.h"

namespace diffink {

class MerkleTree {
private:
  std::unique_ptr<HashNode> Root;
  std::unique_ptr<TSTree, decltype(&ts_tree_delete)> RawTree;

  std::vector<std::pair<const HashNode *, const HashNode *>> Mapping;
  std::unordered_set<const HashNode *> ChangedNodes;
  std::unordered_set<const HashNode *> HasChangedChild;

private:
  struct Iterator {
    const HashNode &OldHashIter;
    TSTreeCursor &OldCursor;
    const HashNode &NewHashIter;
    TSTreeCursor &NewCursor;
  };

  void identifyChange(MerkleTree &OldTree, Iterator Iters);

  void clearChanges() noexcept;

public:
  MerkleTree() noexcept : RawTree(nullptr, ts_tree_delete) {}

  void swap(MerkleTree &Rhs) noexcept;

  void clear() noexcept;

  // Return false if the tree has an error node
  bool parse(TSParser &Parser, const SourceCode &Code);

  // Return false if the tree has an error node
  bool parseIncrementally(TSParser &Parser, MerkleTree &OldTree,
                          const SourceCode &OldCode, const SourceCode &Code,
                          const EditSequence &Seq);

  const HashNode &getRoot() const noexcept { return *Root; }

  const decltype(Mapping) &getMapping() const noexcept { return Mapping; }

  bool isChanged(const HashNode &Node) const noexcept {
    return ChangedNodes.contains(&Node);
  }

  bool hasChangedChild(const HashNode &Node) const noexcept {
    return HasChangedChild.contains(&Node);
  }
};

} // namespace diffink

#endif // AST_MERKLETREE_H