#ifndef DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H
#define DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H

#include "DiffInk/TreeDiff/GumTree/Comparator.h"
#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/TreeDiff.h"
#include <map>

namespace diffink::gumtree {

class GreedyTopDown : public TreeDiff::Matcher {
private:
  const std::size_t MinHeight;

public:
  GreedyTopDown(std::size_t MinHeight = gumtree::DefaultMinHeight,
                float MinDice = gumtree::DefaultMinDice) noexcept
      : TreeDiff::Matcher(), MinHeight{MinHeight} {}

  ~GreedyTopDown() final = default;

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &New) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_GREEDYTOPDOWN_H