#include "DiffInk/TreeDiff/GumTree/OptimalBottomUp.h"

namespace diffink::gumtree {

void OptimalBottomUp::match(TreeDiff &Mapping, VirtualNode *Node) {
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
        if (Seed->Original.getType() == Node->Original.getType() &&
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
    auto Similarity =
        metric::computeDiceSimilarity(Mapping, Desendants, Candidate);
    if (Similarity > Max && Similarity >= MinDice) {
      Max = Similarity;
      Best = Candidate;
    }
  }

  if (Best) {
    Mapping.insertMapping(Node, Best);
    Recovery.match(Mapping, Node, Best);
  }
}

void OptimalBottomUp::match(TreeDiff &Mapping,
                            const std::vector<VirtualNode *> &Old,
                            const std::vector<VirtualNode *> &) {
  for (auto Subtree : Old) {
    if (Subtree != Mapping.getOldRoot())
      Subtree->traversePostOrder(Subtree, [this, &Mapping](VirtualNode *Node) {
        match(Mapping, Node);
      });
    else {
      Mapping.insertMapping(Subtree, Mapping.getNewRoot());
      Recovery.match(Mapping, Subtree, Mapping.getNewRoot());
    }
  }
}

} // namespace diffink::gumtree