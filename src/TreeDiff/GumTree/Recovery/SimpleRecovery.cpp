#include "DiffInk/TreeDiff/GumTree/Recovery/SimpleRecovery.h"

namespace diffink {

namespace gumtree {

template <class Comparator>
void SimpleRecovery::matchLcs(TreeDiff &Mapping, VirtualNode *Old,
                              VirtualNode *New, Comparator &&Comp) const {
  std::vector<VirtualNode *> UnmappedOldChildren, UnmappedNewChildren;
  for (auto Child : Old->Children)
    if (!Mapping.findOldToNewMapping(Child))
      UnmappedOldChildren.push_back(Child);
  for (auto Child : New->Children)
    if (!Mapping.findNewToOldMapping(Child))
      UnmappedNewChildren.push_back(Child);

  const std::size_t N{UnmappedOldChildren.size()};
  const std::size_t M{UnmappedNewChildren.size()};

  std::vector<std::vector<std::size_t>> LcsTable(
      N + 1, std::vector<std::size_t>(M + 1, 0));

  for (std::size_t i{1}; i != N + 1; ++i)
    for (std::size_t j{1}; j != M + 1; ++j)
      LcsTable[i][j] = Comp(UnmappedOldChildren[i - 1]->Original,
                            UnmappedNewChildren[j - 1]->Original)
                           ? LcsTable[i - 1][j - 1] + 1
                           : std::max(LcsTable[i - 1][j], LcsTable[i][j - 1]);

  for (std::size_t i = N, j = M; i != 0 && j != 0;) {
    if (LcsTable[i][j] == LcsTable[i - 1][j - 1] + 1 &&
        Comp(UnmappedOldChildren[i - 1]->Original,
             UnmappedNewChildren[j - 1]->Original)) {
      --i;
      --j;
      VirtualNode::traversePostOrder(
          UnmappedOldChildren[i], UnmappedNewChildren[j],
          [&Mapping](VirtualNode *Old, VirtualNode *New) {
            Mapping.overrideMapping(Old, New);
          });
      continue;
    }

    else if (LcsTable[i][j] == LcsTable[i][j - 1])
      --j;
    else
      --i;
  }
}

void SimpleRecovery::matchUnique(TreeDiff &Mapping, VirtualNode *Old,
                                 VirtualNode *New) {
  std::unordered_set<TSSymbol> Keys;
  std::unordered_map<TSSymbol, std::vector<VirtualNode *>> OldTypes;
  std::unordered_map<TSSymbol, std::vector<VirtualNode *>> NewTypes;

  for (auto Child : Old->Children)
    if (!Mapping.findOldToNewMapping(Child)) {
      OldTypes[Child->Original.getSymbol()].push_back(Child);
      Keys.insert(Child->Original.getSymbol());
    }

  for (auto Child : New->Children)
    if (!Mapping.findNewToOldMapping(Child)) {
      NewTypes[Child->Original.getSymbol()].push_back(Child);
      Keys.insert(Child->Original.getSymbol());
    }

  for (auto Key : Keys) {
    auto OldIter = OldTypes.find(Key);
    auto NewIter = NewTypes.find(Key);
    if (OldIter != OldTypes.cend() && NewIter != NewTypes.cend()) {
      auto &OldCandidates = OldIter->second;
      auto &NewCandidates = NewIter->second;

      if (OldCandidates.size() == 1 && NewCandidates.size() == 1) {
        Mapping.insertMapping(OldCandidates[0], NewCandidates[0]);
        match(Mapping, OldCandidates[0], NewCandidates[0]);
      }
    }
  }
}

void SimpleRecovery::match(TreeDiff &Mapping, VirtualNode *Old,
                           VirtualNode *New) {
  matchLcs(Mapping, Old, New, HashNode::isExactlyEqual);
  matchLcs(Mapping, Old, New, HashNode::isStructurallyEqual);
  matchUnique(Mapping, Old, New);
}

} // namespace gumtree

} // namespace diffink