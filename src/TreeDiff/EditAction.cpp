#include "TreeDiff/EditAction.h"

namespace diffink::edit_action {

std::string InsertNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("insert-node\n").append(SmallSeperator);
  Buffer.append(Leaf->toString()).append("\nto\n").append(Parent->toString());
  Buffer.append("\nat ").append(std::to_string(Index)).push_back('\n');
  return Buffer;
}

std::string InsertTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("insert-tree\n").append(SmallSeperator);
  Buffer.append(Tree->toStringRecursively()).append("\nto\n");
  Buffer.append(Parent->toString()).append("\nat ");
  Buffer.append(std::to_string(Index)).push_back('\n');
  return Buffer;
}

std::string DeleteNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("delete-node\n");
  Buffer.append(SmallSeperator).append(Leaf->toString()).push_back('\n');
  return Buffer;
}

std::string DeleteTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("delete-tree\n");
  Buffer.append(SmallSeperator).append(Tree->toStringRecursively());
  Buffer.push_back('\n');
  return Buffer;
}

std::string MoveTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("move-tree\n").append(SmallSeperator);
  Buffer.append(Tree->toStringRecursively()).append("\nto\n");
  Buffer.append(Parent->toString()).append("\nat ");
  Buffer.append(std::to_string(Index)).push_back('\n');
  return Buffer;
}

std::string UpdateNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("update-node\n").append(SmallSeperator);
  Buffer.append(Leaf->toString()).append("\nreplace ");
  Buffer.append(Leaf->getTextValue()).append(" by ");
  Buffer.append(UpdatedLeaf->getTextValue()).push_back('\n');
  return Buffer;
}

} // namespace diffink::edit_action