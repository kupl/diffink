#ifndef DIFFINK_TREEDIFF_COMPOSITEMATCHER_H
#define DIFFINK_TREEDIFF_COMPOSITEMATCHER_H

#include "DiffInk/TreeDiff/GumTree/GreedyTopDown.h"
#include "DiffInk/TreeDiff/GumTree/OptimalBottomUp.h"
#include "DiffInk/TreeDiff/GumTree/SimpleBottomUp.h"
#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink {

class CompositeMatcher : public TreeDiff::Matcher {
private:
  std::list<std::unique_ptr<TreeDiff::Matcher>> Matchers;

public:
  CompositeMatcher() noexcept = default;

  void addMatcher(std::unique_ptr<TreeDiff::Matcher> Matcher) {
    Matchers.push_back(std::move(Matcher));
  }

  void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
             const std::vector<VirtualNode *> &New);
};

std::unique_ptr<TreeDiff::Matcher>
makeGumtreeOptimal(std::size_t MaxSize = gumtree::DefaultMaxSize,
                   std::size_t MinHeight = gumtree::DefaultMinHeight,
                   double TopDownThreshold = gumtree::DefaultMinDice,
                   double BottomUpThreshold = gumtree::DefaultMinDice);

std::unique_ptr<TreeDiff::Matcher>
makeGumtreeSimple(std::size_t MinHeight = gumtree::DefaultMinHeight,
                  double TopDownThreshold = gumtree::DefaultMinDice,
                  std::optional<double> BottomUpThreshold = std::nullopt);

} // namespace diffink

#endif // DIFFINK_TREEDIFF_COMPOSITEMATCHER_H