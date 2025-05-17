#include "DiffInk/TreeDiff/GumTree/Comparator.h"

namespace diffink::gumtree {

const std::unordered_set<VirtualNode *> &
Comparator::makeSiblings(VirtualNode *Node) {
  auto Iter = SiblingsCache.find(Node);
  if (Iter != SiblingsCache.cend())
    return Iter->second;
  return SiblingsCache.emplace(Node, Node->makeDescendants()).first->second;
}

const std::unordered_set<VirtualNode *> &
Comparator::makeAncestors(VirtualNode *Node) {
  auto Iter = AncestorsCache.find(Node);
  if (Iter != AncestorsCache.cend())
    return Iter->second;

  auto &Temp = AncestorsCache.emplace(Node, std::unordered_set<VirtualNode *>{})
                   .first->second;
  Temp.reserve(Node->Original.getSize());
  for (; Node->Parent != nullptr; Node = Node->Parent)
    Temp.insert(Node);
  return Temp;
}

double Comparator::computeSiblingSimilarity(NodePair Pair) {
  if (Pair.first->Parent == Pair.second->Parent)
    return 1.0;
  auto ParentPair = std::make_pair(Pair.first->Parent, Pair.second->Parent);

  auto Iter = SiblingSimilarityCache.find(ParentPair);
  if (Iter != SiblingSimilarityCache.cend())
    return Iter->second;

  auto Dice = metric::computeDiceSimilarity(
      Mapping, makeSiblings(ParentPair.first), ParentPair.second);
  SiblingSimilarityCache.emplace(ParentPair, Dice);
  return Dice;
}

double Comparator::computeAncestorSimilarity(NodePair Pair) {
  if (Pair.first->Parent == Pair.second->Parent)
    return 1.0;
  auto ParentPair = std::make_pair(Pair.first->Parent, Pair.second->Parent);

  auto Iter = AncestorSimilarityCache.find(ParentPair);
  if (Iter != AncestorSimilarityCache.cend())
    return Iter->second;

  auto &OldAncestors = makeAncestors(ParentPair.first);
  auto &NewAncestors = makeAncestors(ParentPair.second);

  auto Dice =
      metric::computeDiceSimilarity(Mapping, OldAncestors, NewAncestors);
  AncestorSimilarityCache.emplace(ParentPair, Dice);
  return Dice;
}

int64_t Comparator::computePositionSimilarity(NodePair Pair) {
  std::vector<VirtualNode *> OldPositions, NewPositions;

  for (auto Node = Pair.first; Node->Parent != nullptr; Node = Node->Parent)
    OldPositions.push_back(Node);
  for (auto Node = Pair.second; Node->Parent != nullptr; Node = Node->Parent)
    NewPositions.push_back(Node);

  int64_t SquareSum{0};

  for (auto OldIter = OldPositions.begin(), NewIter = NewPositions.begin();
       OldIter != OldPositions.end() && NewIter != NewPositions.end();
       ++OldIter, ++NewIter) {
    auto OldIndex = static_cast<int64_t>((*OldIter)->findChild(*OldIter));
    auto NewIndex = static_cast<int64_t>((*NewIter)->findChild(*NewIter));
    SquareSum -= (OldIndex - NewIndex) * (OldIndex - NewIndex);
  }
  return SquareSum;
}

int64_t Comparator::computeTextualSimilarity(NodePair Pair) {
  auto OldPos = Pair.first->Original.getByteRange();
  auto NewPos = Pair.second->Original.getByteRange();
  return -std::abs(static_cast<int64_t>(OldPos.first) -
                   static_cast<int64_t>(NewPos.first)) -
         std::abs(static_cast<int64_t>(OldPos.second) -
                  static_cast<int64_t>(NewPos.second));
}

bool Comparator::operator()(NodePair Left, NodePair Right) {
  {
    auto LeftSimilarity = computeSiblingSimilarity(Left);
    auto RightSimilarity = computeSiblingSimilarity(Right);
    if (LeftSimilarity > RightSimilarity)
      return true;
    if (LeftSimilarity != RightSimilarity)
      return false;
  }
  {
    auto LeftSimilarity = computeAncestorSimilarity(Left);
    auto RightSimilarity = computeAncestorSimilarity(Right);
    if (LeftSimilarity > RightSimilarity)
      return true;
    if (LeftSimilarity != RightSimilarity)
      return false;
  }
  {
    auto LeftSimilarity = computePositionSimilarity(Left);
    auto RightSimilarity = computePositionSimilarity(Right);
    if (LeftSimilarity > RightSimilarity)
      return true;
    if (LeftSimilarity != RightSimilarity)
      return false;
  }
  {
    auto LeftSimilarity = computeTextualSimilarity(Left);
    auto RightSimilarity = computeTextualSimilarity(Right);
    if (LeftSimilarity > RightSimilarity)
      return true;
    return false;
  }
}

} // namespace diffink::gumtree