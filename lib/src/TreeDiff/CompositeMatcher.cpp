#include "DiffInk/TreeDiff/CompositeMatcher.h"

namespace diffink {

void CompositeMatcher::match(TreeDiff &Mapping,
                             const std::vector<VirtualNode *> &Old,
                             const std::vector<VirtualNode *> &New) {
  for (const auto &Matcher : Matchers)
    Matcher->match(Mapping, Old, New);
}

std::unique_ptr<TreeDiff::Matcher>
makeGumtreeOptimal(std::size_t MaxSize, std::size_t MinHeight,
                   double TopDownThreshold, double BottomUpThreshold) {
  auto Matcher = std::make_unique<CompositeMatcher>();
  Matcher->addMatcher(
      std::make_unique<gumtree::GreedyTopDown>(MinHeight, TopDownThreshold));
  Matcher->addMatcher(
      std::make_unique<gumtree::OptimalBottomUp>(MaxSize, BottomUpThreshold));
  return Matcher;
}

std::unique_ptr<TreeDiff::Matcher>
makeGumtreeSimple(std::size_t MinHeight, double TopDownThreshold,
                  std::optional<double> BottomUpThreshold) {
  auto Matcher = std::make_unique<CompositeMatcher>();
  Matcher->addMatcher(
      std::make_unique<gumtree::GreedyTopDown>(MinHeight, TopDownThreshold));
  Matcher->addMatcher(
      std::make_unique<gumtree::SimpleBottomUp>(BottomUpThreshold));
  return Matcher;
}

} // namespace diffink
