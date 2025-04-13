#include "AST/MerkleTree.h"

namespace diffink {

void MerkleTree::build(const RawTree &Tree, const HashNode *Parent,
                       TSTreeCursor &Cursor) {
  auto CurrentNode = ts_tree_cursor_current_node(&Cursor);
  auto CurrentHashNode =
      &NodeStorage.emplace_back(Parent, Tree.getNodeInfo(CurrentNode));

  if (!ts_tree_cursor_goto_first_child(&Cursor))
    ts_tree_cursor_goto_parent(&Cursor);
  else
    do
      build(Tree, CurrentHashNode, Cursor);
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
}

void MerkleTree::build(const RawTree &Tree) {
  NodeStorage.clear();
  decltype(ChangedRanges)().swap(ChangedRanges);
  ChangedRanges = Tree.getChangedRanges();

  auto RootNode = Tree.getRootNode();
  auto Cursor = ts_tree_cursor_new(RootNode);
  build(Tree, nullptr, Cursor);
  ts_tree_cursor_delete(&Cursor);

  if (NodeStorage.empty())
    throw std::runtime_error("Empty AST");
  Root = &NodeStorage.front();
  Root->setMetadataRecursively();
}

HashNode::Set MerkleTree::getFullTree() const {
  HashNode::Set Result;
  for (auto Child : Root->getChildren())
    Result.push_back(Child);
  return Result;
}

} // namespace diffink