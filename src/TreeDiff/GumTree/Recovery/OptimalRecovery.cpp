#include "DiffInk/TreeDiff/GumTree/Recovery/OptimalRecovery.h"

namespace diffink {

namespace gumtree {

int OptimalRecovery::computeSimilarity(VirtualNode *Left,
                                       VirtualNode *Right) const {
  if (Left->Original.getSymbol() != Right->Original.getSymbol())
    return std::numeric_limits<int>::max();
  return HashNode::isExactlyEqual(Left->Original, Right->Original) ? 0 : 1;
}

void OptimalRecovery::tryMapping(TreeDiff &Mapping, VirtualNode *Old,
                                 VirtualNode *New) const {
  if (!Mapping.findOldToNewMapping(Old) && !Mapping.findNewToOldMapping(New))
    Mapping.insertMapping(Old, New);
}

void OptimalRecovery::match(TreeDiff &Mapping, VirtualNode *Old,
                            VirtualNode *New) {
  if (std::max(Old->Original.getSize(), New->Original.getSize()) > MaxSize)
    return;

  auto OldPostOrder = Old->makePostOrder();
  auto NewPostOrder = New->makePostOrder();

  std::unordered_map<VirtualNode *, int> OldIndex, NewIndex;
  OldIndex.reserve(OldPostOrder.size());
  NewIndex.reserve(NewPostOrder.size());
  std::vector<int> OldLeftMost(OldPostOrder.size(), 0);
  std::vector<int> NewLeftMost(NewPostOrder.size(), 0);

  for (int i{0}; i != OldPostOrder.size(); ++i) {
    auto Node = OldPostOrder[i];
    OldIndex.emplace(Node, i);
    OldLeftMost[i] =
        Node->Original.isLeaf() ? i : OldLeftMost[OldIndex[Node->Children[0]]];
  }
  for (int i{0}; i != NewPostOrder.size(); ++i) {
    auto Node = NewPostOrder[i];
    NewIndex.emplace(Node, i);
    NewLeftMost[i] =
        Node->Original.isLeaf() ? i : NewLeftMost[NewIndex[Node->Children[0]]];
  }

  std::vector<int> OldKeys, NewKeys;
  {
    std::unordered_set<int> Visited;
    for (int i = OldPostOrder.size() - 1; i != -1; --i)
      if (Visited.insert(OldLeftMost[i]).second)
        OldKeys.push_back(i);
    std::reverse(OldKeys.begin(), OldKeys.end());
  }
  {
    std::unordered_set<int> Visited;
    for (int i = NewPostOrder.size() - 1; i != -1; --i)
      if (Visited.insert(NewLeftMost[i]).second)
        NewKeys.push_back(i);
    std::reverse(NewKeys.begin(), NewKeys.end());
  }

  const int N = OldPostOrder.size();
  const int M = NewPostOrder.size();

  std::vector<std::vector<int>> TreeDist(N + 1, std::vector<int>(M + 1, 0));
  std::vector<std::vector<int>> ForestDist(N + 1, std::vector<int>(M + 1, 0));

  auto LldOld = [&OldLeftMost](int Index) {
    return OldLeftMost[Index - 1] + 1;
  };
  auto LldNew = [&NewLeftMost](int Index) {
    return NewLeftMost[Index - 1] + 1;
  };

  for (auto OldKey : OldKeys) {
    auto OldL = OldLeftMost[OldKey] + 1;
    ++OldKey;

    for (auto NewKey : NewKeys) {
      auto NewL = NewLeftMost[NewKey] + 1;
      ++NewKey;

      ForestDist[OldL - 1][NewL - 1] = 0;
      for (int i{OldL}; i <= OldKey; ++i)
        ForestDist[i][NewL - 1] = ForestDist[i - 1][NewL - 1] + 1;

      for (int j{NewL}; j <= NewKey; ++j)
        ForestDist[OldL - 1][j] = ForestDist[OldL - 1][j - 1] + 1;

      for (int i{OldL}; i <= OldKey; ++i) {
        for (int j{NewL}; j <= NewKey; ++j) {
          auto CostDel = ForestDist[i - 1][j] + 1;
          auto CostIns = ForestDist[i][j - 1] + 1;

          if (LldOld(i) == OldL && LldNew(j) == NewL) {
            auto Similarity =
                computeSimilarity(OldPostOrder[i - 1], NewPostOrder[j - 1]);
            auto CostUpd = (Similarity == std::numeric_limits<int>::max())
                               ? std::numeric_limits<int>::max()
                               : ForestDist[i - 1][j - 1] + Similarity;
            ForestDist[i][j] = std::min({CostDel, CostIns, CostUpd});
            TreeDist[i][j] = ForestDist[i][j];
          }

          else
            ForestDist[i][j] = std::min(
                {CostDel, CostIns,
                 ForestDist[LldOld(i) - 1][LldNew(j) - 1] + TreeDist[i][j]});
        }
      }
      TreeDist[OldKey][NewKey] = ForestDist[OldKey][NewKey];
    }

    std::stack<std::pair<int, int>> Stack;
    Stack.emplace(N, M);

    while (!Stack.empty()) {
      const auto [LastRow, LastCol] = Stack.top();
      Stack.pop();
      const auto FirstRow = LldOld(LastRow) - 1;
      const auto FirstCol = LldNew(LastCol) - 1;

      for (auto Row = LastRow, Col = LastCol;
           Row > FirstRow || Col > FirstCol;) {
        if (Row > FirstRow &&
            ForestDist[Row - 1][Col] + 1 == ForestDist[Row][Col])
          --Row;
        else if (Col > FirstCol &&
                 ForestDist[Row][Col - 1] + 1 == ForestDist[Row][Col])
          --Col;

        else {
          if (LldOld(Row) == LldOld(LastRow) &&
              LldNew(Col) == LldNew(LastCol)) {
            if (OldPostOrder[Row - 1]->Original.getSymbol() ==
                NewPostOrder[Col - 1]->Original.getSymbol()) {
              tryMapping(Mapping, OldPostOrder[Row - 1], NewPostOrder[Col - 1]);
              --Row;
              --Col;
            } else
              throw std::runtime_error(
                  "Unreachable code in OptimalRecovery::match");
          } else {
            Stack.emplace(Row, Col);
            Row = LldOld(Row) - 1;
            Col = LldNew(Col) - 1;
          }
        }
      }
    }
  }
}

} // namespace gumtree

} // namespace diffink