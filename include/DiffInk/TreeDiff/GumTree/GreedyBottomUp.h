#ifndef DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H
#define DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H

#include "DiffInk/TreeDiff/GumTree/DiceSimilarity.h"
#include "DiffInk/TreeDiff/GumTree/HyperParameters.h"
#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::gumtree {

class GreedyBottomUp : public TreeDiff::Matcher {
public:
  class Recovery {
  public:
    virtual void match(TreeDiff &Mapping, VirtualNode *Old,
                       VirtualNode *New) = 0;
  };

private:
  std::unique_ptr<Recovery> Heuristic;
  const double MinDice;

private:
  void match(TreeDiff &Mapping, VirtualNode *Node);

public:
  GreedyBottomUp(std::unique_ptr<Recovery> Heuristic,
                 double MinDice = DefaultMinDice) noexcept
      : Heuristic(std::move(Heuristic)), MinDice{MinDice} {}

  ~GreedyBottomUp() final = default;

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &) final;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_GREEDYBOTTOMUP_H