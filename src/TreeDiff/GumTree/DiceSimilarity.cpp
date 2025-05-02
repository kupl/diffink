#include "DiffInk/TreeDiff/GumTree/DiceSimilarity.h"

namespace diffink::gumtree {

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             const std::unordered_set<VirtualNode *> &New) {
  std::size_t Count{0};
  if (Old.size() < New.size()) {
    for (auto Node : Old)
      if (New.contains(Mapping.findOldToNewMapping(Node)))
        ++Count;
  } else
    for (auto Node : New)
      if (Old.contains(Mapping.findNewToOldMapping(Node)))
        ++Count;
  return static_cast<double>(2 * Count) / (Old.size() + New.size());
}

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             VirtualNode *New) {
  std::size_t Count{0};
  VirtualNode::traversePostOrder(New, [&, New](VirtualNode *Node) {
    if (Node != New && Old.contains(Mapping.findNewToOldMapping(Node)))
      ++Count;
  });
  return static_cast<double>(2 * Count) /
         (Old.size() + New->Original.getSize() - 1);
}

} // namespace diffink::gumtree