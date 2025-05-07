#ifndef DIFFINK_TREEDIFF_GUMTREE_OPTIMALRECOVERY_H
#define DIFFINK_TREEDIFF_GUMTREE_OPTIMALRECOVERY_H

#include "DiffInk/TreeDiff/GumTree/GreedyBottomUp.h"
#include <limits>
#include <stack>

namespace diffink::gumtree {

class OptimalRecovery : public GreedyBottomUp::Recovery {
private:
  const int MaxSize;

private:
  int computeUpdateCost(VirtualNode *Left, VirtualNode *Right) const;

  void tryMapping(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) const;

public:
  OptimalRecovery(int MaxSize) noexcept
      : GreedyBottomUp::Recovery(), MaxSize{MaxSize} {}

  ~OptimalRecovery() = default;

  void match(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_OPTIMALRECOVERY_H