#ifndef DIFFINK_TREEDIFF_GUMTREE_OPTIMALBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_OPTIMALBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/Metric/DiceSimilarity.h"
#include "DiffInk/TreeDiff/Recovery/OptimalRecovery.h"

namespace diffink::gumtree {

class OptimalBottomUp : public TreeDiff::Matcher {
private:
  const double MinDice;
  recovery::OptimalRecovery Recovery;

private:
  void match(TreeDiff &Mapping, VirtualNode *Node);

public:
  OptimalBottomUp(uint32_t MaxSize = DefaultMaxSize,
                  double MinDice = DefaultMinDice) noexcept
      : MinDice{MinDice}, Recovery(DefaultMaxSize) {}

  ~OptimalBottomUp() final = default;

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_OPTIMALBOTTOMUP_H