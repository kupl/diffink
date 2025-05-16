#ifndef DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/GumTree/Recovery/SimpleRecovery.h"

namespace diffink::gumtree {

class SimpleBottomUp : public TreeDiff::Matcher {
private:
  const double MinDice;

private:
  void match(TreeDiff &Mapping, VirtualNode *Node);

public:
  SimpleBottomUp(double MinDice = DefaultMinDice) noexcept : MinDice{MinDice} {}

  ~SimpleBottomUp() final = default;

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_SIMPLEBOTTOMUP_H