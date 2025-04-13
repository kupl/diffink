#ifndef TREEDIFF_EDITACTION_H
#define TREEDIFF_EDITACTION_H

#include "AST/HashNode.h"
#include <variant>

namespace diffink {

constexpr std::string_view BigSeperator = "===\n";
constexpr std::string_view SmallSeperator = "---\n";

namespace edit_action {

struct InsertNode {
  const HashNode *Leaf;
  const HashNode *Parent;
  std::size_t Index;

  std::string toString() const;
};

struct DeleteNode {
  const HashNode *Leaf;

  std::string toString() const;
};

struct MoveTree {
  const HashNode *Tree;
  const HashNode *Parent;
  std::size_t Index;

  std::string toString() const;
};

struct UpdateNode {
  const HashNode *Leaf;
  const HashNode *UpdatedLeaf;

  std::string toString() const;
};

} // namespace edit_action

using EditAction =
    std::variant<edit_action::InsertNode, edit_action::DeleteNode,
                 edit_action::MoveTree, edit_action::UpdateNode>;

} // namespace diffink

#endif // TREEDIFF_EDITACTION_H