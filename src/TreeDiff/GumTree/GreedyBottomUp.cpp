#include "DiffInk/TreeDiff/GumTree/GreedyBottomUp.h"

namespace diffink {

namespace gumtree {

void GreedyBottomUp::match(TreeDiff &Mapping, VirtualNode *Node) {
  if (Node->Original.isLeaf() || Mapping.findOldToNewMapping(Node))
    return;

  std::vector<VirtualNode *> Seeds;
  VirtualNode::traversePostOrder(
      Node, [this, &Mapping, &Seeds](VirtualNode *Node) {
        auto Partner = Mapping.findOldToNewMapping(Node);
        if (Partner)
          Seeds.push_back(Partner);
      });

  std::vector<VirtualNode *> Candidates;
  {
    std::unordered_set<VirtualNode *> Visited;
    for (auto Seed : Seeds)
      for (Seed = Seed->Parent;
           Seed->Parent != nullptr && !Visited.contains(Seed);
           Seed = Seed->Parent) {
        Visited.insert(Seed);
        if (Seed->Original.getSymbol() == Node->Original.getSymbol() &&
            !Mapping.findNewToOldMapping(Seed))
          Candidates.push_back(Seed);
      }
  }

  if (Candidates.empty())
    return;
  auto Desendants = Node->makeDescendants();
  VirtualNode *Best{nullptr};
  double Max{-1.0};

  for (auto Candidate : Candidates) {
    auto Similarity = computeDiceSimilarity(Mapping, Desendants, Candidate);
    if (Similarity > Max && Similarity > MinDice) {
      Max = Similarity;
      Best = Candidate;
    }
  }

  if (Best) {
    Mapping.insertMapping(Node, Best);
    if (Heuristic)
      Heuristic->match(Mapping, Node, Best);
  }
}

void GreedyBottomUp::match(TreeDiff &Mapping,
                           const std::vector<VirtualNode *> &Old,
                           const std::vector<VirtualNode *> &) {
  for (auto Subtree : Old) {
    if (Subtree != Mapping.getOldRoot())
      Subtree->traversePostOrder(Subtree, [this, &Mapping](VirtualNode *Node) {
        match(Mapping, Node);
      });
    else {
      Mapping.insertMapping(Subtree, Mapping.getNewRoot());
      if (Heuristic)
        Heuristic->match(Mapping, Subtree, Mapping.getNewRoot());
    }
  }
}

} // namespace gumtree

} // namespace diffink