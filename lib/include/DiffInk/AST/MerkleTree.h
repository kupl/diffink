#ifndef DIFFINK_AST_MERKLETREE_H
#define DIFFINK_AST_MERKLETREE_H

#include "DiffInk/TreeDiff/EditAction.h"
#include "DiffInk/Utils/NodeComp.h"
#include "DiffInk/Utils/TextDiff.h"

namespace diffink {

class MerkleTree {
private:
  std::unique_ptr<HashNode> Root;
  std::unique_ptr<TSTree, decltype(&ts_tree_delete)> RawTree;
  // HashNode::BuildConfig Config;

  bool Incparsed{false};
  std::vector<std::pair<const HashNode *, const HashNode *>> Mapping;
  std::unordered_set<const HashNode *> UncommonNodes;
  std::unordered_set<const HashNode *> HasUncommonChild;

private:
  struct Iterator {
    const HashNode &OldHashIter;
    TSTreeCursor &OldCursor;
    const HashNode &NewHashIter;
    TSTreeCursor &NewCursor;
  };

  void identifyChange(MerkleTree &OldTree, Iterator Iters);

  void clearMapping() noexcept;

public:
  MerkleTree() noexcept : RawTree(nullptr, ts_tree_delete) {}

  void swap(MerkleTree &Rhs) noexcept;

  void clear() noexcept;

  void parse(TSParser &Parser, const SourceCode &Code);

  void incparse(TSParser &Parser, MerkleTree &OldTree,
                const SourceCode &OldCode, const SourceCode &Code);

  // void setFlattened(const std::unordered_set<std::string> &Flattened) {
  //   Config.Flattened = Flattened;
  // }

  // void setAliased(const std::unordered_set<std::string> &Aliased) {
  //   Config.Aliased = Aliased;
  // }

  // void setIgnored(const std::unordered_set<std::string> &Ignored) {
  //   Config.Ignored = Ignored;
  // }

  const HashNode &getRoot() const noexcept { return *Root; }

  bool isIncparsed() const noexcept { return Incparsed; }

  const decltype(Mapping) &getMapping() const noexcept { return Mapping; }

  bool isUncommon(const HashNode &Node) const {
    return UncommonNodes.contains(&Node);
  }

  bool hasUncommonChild(const HashNode &Node) const {
    return HasUncommonChild.contains(&Node);
  }
};

} // namespace diffink

#endif // DIFFINK_AST_MERKLETREE_H