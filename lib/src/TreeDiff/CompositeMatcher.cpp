#include "DiffInk/TreeDiff/CompositeMatcher.h"

namespace diffink {

void CompositeMatcher::match(TreeDiff &Mapping,
                             const std::vector<VirtualNode *> &Old,
                             const std::vector<VirtualNode *> &New) {
  for (const auto &Matcher : Matchers)
    Matcher->match(Mapping, Old, New);
}

std::unique_ptr<TreeDiff::Matcher> makeGumtreeOptimal(std::size_t MaxSize,
                                                      std::size_t MinHeight,
                                                      double TopDownMinDice,
                                                      double BottomUpMinDice) {
  auto Matcher = std::make_unique<CompositeMatcher>();
  Matcher->addMatcher(
      std::make_unique<gumtree::GreedyTopDown>(MinHeight, TopDownMinDice));
  Matcher->addMatcher(std::make_unique<gumtree::GreedyBottomUp>(
      std::make_unique<gumtree::OptimalRecovery>(MaxSize), BottomUpMinDice));
  return Matcher;
}

std::unique_ptr<TreeDiff::Matcher> makeGumtreeSimple(std::size_t MinHeight,
                                                     double TopDownMinDice,
                                                     double BottomUpMinDice) {
  auto Matcher = std::make_unique<CompositeMatcher>();
  Matcher->addMatcher(
      std::make_unique<gumtree::GreedyTopDown>(MinHeight, TopDownMinDice));
  Matcher->addMatcher(std::make_unique<gumtree::GreedyBottomUp>(
      std::make_unique<gumtree::SimpleRecovery>(), BottomUpMinDice));
  return Matcher;
}

} // namespace diffink
