#ifndef AST_RAWTREE_H
#define AST_RAWTREE_H

#include "AST/RawNode.h"
#include "Parser/SourceCode.h"

namespace diffink {

class RawTree {
private:
  std::unique_ptr<TSTree, decltype(&ts_tree_delete)> Tree;
  std::unique_ptr<SourceCode> Code;
  std::vector<TSRange> ChangedRanges;

public:
  RawTree() noexcept : Tree(nullptr, ts_tree_delete) {}

  RawTree(RawTree &&Rhs) noexcept
      : Tree(std::move(Rhs.Tree)), Code(std::move(Rhs.Code)) {}

  void swap(RawTree &Rhs) noexcept;

  void parse(TSParser &Parser, std::unique_ptr<SourceCode> Code) noexcept;

  void parse(TSParser &Parser, std::unique_ptr<SourceCode> Code,
             const RawTree &OldTree,
             const std::vector<TSInputEdit> &EditSequence);

  TSNode getRootNode() const noexcept { return ts_tree_root_node(Tree.get()); }

  const std::vector<TSRange> &getChangedRanges() const noexcept {
    return ChangedRanges;
  }

  RawNode getNodeInfo(TSNode RawNode) const noexcept;
};

} // namespace diffink

#endif // AST_RAWTREE_H