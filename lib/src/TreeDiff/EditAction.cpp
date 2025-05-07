#include "DiffInk/TreeDiff/EditAction.h"

namespace diffink {

namespace edit_action {

std::string InsertNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("insert-node\n").append(SmallSeperator);
  Buffer.append(Leaf.toString()).append("\nto\n").append(Parent.toString());
  Buffer.append("\nat ").append(std::to_string(Index + 1)).push_back('\n');
  return Buffer;
}

std::string DeleteNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("delete-node\n");
  Buffer.append(SmallSeperator).append(Leaf.toString()).push_back('\n');
  return Buffer;
}

std::string MoveTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("move-tree\n").append(SmallSeperator);
  Buffer.append(Subtree.toStringRecursively()).append("\nto\n");
  Buffer.append(Parent.toString()).append("\nat ");
  Buffer.append(std::to_string(Index + 1)).push_back('\n');
  return Buffer;
}

std::string UpdateNode::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("update-node\n").append(SmallSeperator);
  Buffer.append(Leaf.toString()).append("\nreplace \"");
  Buffer.append(Leaf.getLabel()).append("\" by \"");
  Buffer.append(UpdatedLeaf.getLabel()).append("\"\n");
  return Buffer;
}

std::string InsertTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("insert-tree\n").append(SmallSeperator);
  Buffer.append(Subtree.toStringRecursively());
  Buffer.append("\nto\n").append(Parent.toString());
  Buffer.append("\nat ").append(std::to_string(Index + 1)).push_back('\n');
  return Buffer;
}

std::string DeleteTree::toString() const {
  std::string Buffer;
  Buffer.append(BigSeperator).append("delete-tree\n");
  Buffer.append(SmallSeperator);
  Buffer.append(Subtree.toStringRecursively()).push_back('\n');
  return Buffer;
}

} // namespace edit_action

ExtendedEditScript simplifyEditScript(const EditScript &Script) {
  ExtendedEditScript ExScript;
  std::unordered_set<const HashNode *> InsertedNodes;
  std::unordered_set<const HashNode *> DeletedNodes;
  ExScript.reserve(Script.size());
  InsertedNodes.reserve(Script.size());
  DeletedNodes.reserve(Script.size());

  std::unordered_map<const HashNode *, std::vector<const HashNode *>> Cache;

  auto makePostOrder =
      [&Cache](const HashNode &Node) -> const std::vector<const HashNode *> & {
    auto Iter = Cache.find(&Node);
    if (Iter != Cache.cend())
      return Iter->second;
    return Cache.emplace(&Node, Node.makePostOrder()).first->second;
  };

  auto detectAggregation =
      [](const std::vector<const HashNode *> &Descendants,
         std::unordered_set<const HashNode *> &Set) -> bool {
    for (const auto Node : Descendants)
      if (!Set.contains(Node))
        return false;
    return true;
  };

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, edit_action::InsertNode>)
            InsertedNodes.insert(&Action.Leaf);
          else if constexpr (std::is_same_v<T, edit_action::DeleteNode>)
            DeletedNodes.insert(&Action.Leaf);
        },
        Action);
  }

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;

          if constexpr (std::is_same_v<T, edit_action::InsertNode>) {
            auto Descendants = makePostOrder(Action.Leaf);

            if (detectAggregation(Descendants, InsertedNodes)) {
              if (!InsertedNodes.contains(&Action.Parent) ||
                  !detectAggregation(makePostOrder(Action.Parent),
                                     InsertedNodes))
                if (Action.Leaf.isLeaf())
                  ExScript.push_back(Action);
                else
                  ExScript.push_back(
                      edit_action::InsertTree{.Subtree = Action.Leaf,
                                              .Parent = Action.Parent,
                                              .Index = Action.Index});
            }

            else
              ExScript.push_back(Action);
          }

          else if constexpr (std::is_same_v<T, edit_action::DeleteNode>) {
            auto Descendants = makePostOrder(Action.Leaf);

            if (detectAggregation(Descendants, DeletedNodes)) {
              if (!DeletedNodes.contains(&Action.Parent) ||
                  !detectAggregation(makePostOrder(Action.Parent),
                                     DeletedNodes))
                if (Action.Leaf.isLeaf())
                  ExScript.push_back(Action);
                else
                  ExScript.push_back(
                      edit_action::DeleteTree{.Subtree = Action.Leaf});
            }

            else
              ExScript.push_back(Action);
          }

          else
            ExScript.emplace_back(Action);
        },
        Action);
  }

  return ExScript;
}

} // namespace diffink