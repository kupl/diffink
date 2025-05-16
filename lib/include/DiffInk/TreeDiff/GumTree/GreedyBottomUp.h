#ifndef DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/DiceSimilarity.h"
#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/GumTree/Recovery/BaseRecovery.h"

namespace diffink::gumtree {

class GreedyBottomUp : public TreeDiff::Matcher {
private:
  const double MinDice;

private:
  void match(TreeDiff &Mapping, VirtualNode *Node);

public:
  GreedyBottomUp(double MinDice = DefaultMinDice) noexcept : MinDice{MinDice} {}

  ~GreedyBottomUp() final = default;

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H