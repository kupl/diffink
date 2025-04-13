#include "TreeDiff/EditAction.h"

namespace diffink::edit_action {

std::string InsertNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("insert-node\n").append(SmallSeperator);
  Buffer.append(Leaf->toString()).append("to\n").append(Parent->toString());
  Buffer.append("at ").append(std::to_string(Index)).append("\n");
  Buffer.append(BigSeperator);
  return Buffer;
}

std::string DeleteNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("delete-node\n").append(SmallSeperator);
  Buffer.append(Leaf->toString()).append(BigSeperator);
  return Buffer;
}

std::string MoveTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("move-tree\n").append(SmallSeperator);
  Buffer.append(Tree->toStringRecursively()).append("to\n");
  Buffer.append(Parent->toString()).append("at ");
  Buffer.append(std::to_string(Index)).append("\n").append(BigSeperator);
  return Buffer;
}

std::string UpdateNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("update-node\n").append(SmallSeperator);
  Buffer.append(Leaf->toString()).append("replace ");
  Buffer.append(Leaf->getInfo().Text).append(" by ");
  Buffer.append(UpdatedLeaf->getInfo().Text).push_back('\n');
  Buffer.append(BigSeperator);
  return Buffer;
}

} // namespace diffink::edit_action