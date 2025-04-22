#include "TreeDiff/BaseTreeDiff.h"
#include <iostream>

namespace diffink {

void BaseTreeDiff::insertMapping(VirtualTree::Node *OldNode,
                                 VirtualTree::Node *NewNode) {
  OldToNewMapping.emplace(OldNode, NewNode);
  NewToOldMapping.emplace(NewNode, OldNode);
}

void BaseTreeDiff::build(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
                         std::vector<VirtualTree::Node *> &ChangeSet,
                         const HashNode &OriginalNode,
                         VirtualTree::Node *NodeCopy) {
  NodeCopy->VirtualHeight = 1;
  if (OriginalNode.getChildren().empty()) {
    NodeCopy->VirtualHash = OriginalNode.getSymbolHash();
    return;
  }
  NodeCopy->VirtualHeight = 2;

  std::vector<xxh::hash_t<BitMode>> Hashes;
  Hashes.reserve(OriginalNode.getChildren().size());

  for (auto &Child : OriginalNode.getChildren()) {
    auto Temp = TreeCopy.pushBack(Child, NodeCopy);

    if (OriginalTree.isChanged(Child)) {
      Temp->VirtualHash = ChangedSymbolHash;
      ChangeSet.push_back(Temp);
      Hashes.push_back(ChangedSymbolHash);
    }

    else {
      VirtualMap.emplace(&Child, Temp);
      if (OriginalTree.hasChangedChild(Child)) {
        build(OriginalTree, TreeCopy, ChangeSet, Child, Temp);
        Hashes.push_back(Temp->VirtualHash);
        NodeCopy->VirtualHeight =
            std::max(NodeCopy->VirtualHeight, Temp->VirtualHeight + 1);
      }

      else
        Hashes.push_back(Child.getStructuralHash());
    }
  }

  NodeCopy->VirtualHash = xxh::xxhash3<BitMode>(
      {OriginalNode.getSymbolHash(), xxh::xxhash3<BitMode>(Hashes)});
}

void BaseTreeDiff::swapMapping(VirtualTree::Node *NewNode,
                               VirtualTree::Node *AltNewNode) {
  if (auto OldNode = findMapping(NewToOldMapping, NewNode); OldNode)
    if (auto AltOldNode = findMapping(NewToOldMapping, AltNewNode);
        AltOldNode) {
      NewToOldMapping[NewNode] = AltOldNode;
      NewToOldMapping[AltNewNode] = OldNode;
      OldToNewMapping[OldNode] = AltNewNode;
      OldToNewMapping[AltOldNode] = NewNode;
    }
}

VirtualTree::Node *BaseTreeDiff::findMapping(
    const std::unordered_map<VirtualTree::Node *, VirtualTree::Node *> &Mapping,
    VirtualTree::Node *Node) const {
  auto It = Mapping.find(Node);
  if (It != Mapping.cend())
    return It->second;
  return nullptr;
}

bool BaseTreeDiff::areContained(VirtualTree::Node *OldNode,
                                VirtualTree::Node *NewNode) const {
  auto Temp = findMapping(NewToOldMapping, NewNode);
  return Temp == OldNode;
}

EditScript BaseTreeDiff::makeEditScript() {
  EditScript Script;
  std::queue<VirtualTree::Node *> Queue;
  for (auto Node : NewTree.getRoot()->Children)
    Queue.push(Node);

  while (!Queue.empty()) {
    auto Node = Queue.front();
    Queue.pop();
    for (auto Child : Node->Children)
      Queue.push(Child);
    auto Partner = findMapping(NewToOldMapping, Node);
    auto ParentPartner = findMapping(NewToOldMapping, Node->Parent);

    if (!Partner) {
      auto Index = findPosition(Node);
      Script.emplace_back(
          edit_action::InsertNode{.Leaf = Node->Original,
                                  .Parent = Node->Parent->Original,
                                  .Index = Index});
      Partner = OldTree.insert(Node->Original, ParentPartner, Index);
      insertMapping(Partner, Node);
    }

    else {
      if (Partner->Original.getTextValue() != Node->Original.getTextValue())
        Script.emplace_back(edit_action::UpdateNode{
            .Leaf = Partner->Original, .UpdatedLeaf = Node->Original});

      if (ParentPartner != Partner->Parent) {
        auto Index = findPosition(Node);
        Script.emplace_back(
            edit_action::MoveTree{.Tree = Partner->Original,
                                  .Parent = ParentPartner->Original,
                                  .Index = Index});
        OldTree.move(Partner, ParentPartner, Index);
      }

      if (Partner->Children.empty() || Node->Children.empty())
        continue;

      std::vector<std::vector<std::size_t>> LcsTable(
          Partner->Children.size() + 1,
          std::vector<std::size_t>(Node->Children.size() + 1, 0));

      for (std::size_t i{1}; i != Partner->Children.size() + 1; ++i)
        LcsTable[i][0] = i;
      for (std::size_t j{1}; j != Node->Children.size() + 1; ++j)
        LcsTable[0][j] = j;

      for (std::size_t i{1}; i != Partner->Children.size() + 1; ++i)
        for (std::size_t j{1}; j != Node->Children.size() + 1; ++j)
          LcsTable[i][j] =
              areContained(Partner->Children[i - 1], Node->Children[j - 1])
                  ? LcsTable[i - 1][j - 1]
                  : std::min(LcsTable[i - 1][j], LcsTable[i][j - 1]) + 1;

      std::forward_list<VirtualTree::Node *> OutOfOrderNodes;

      for (std::size_t i = Partner->Children.size(), j = Node->Children.size();
           i != 0 && j != 0;) {
        if (LcsTable[i][j] == LcsTable[i - 1][j] + 1)
          --i;

        else if (LcsTable[i][j] == LcsTable[i][j - 1] + 1) {
          --j;
          OutOfOrderNodes.push_front(Node->Children[j]);
        }

        else {
          --i;
          --j;
          Partner->Children[i]->Marker = true;
          Node->Children[j]->Marker = true;
        }
      }

      if (!OutOfOrderNodes.empty()) {
        std::unordered_set<VirtualTree::Node *> PartnerChildren;
        PartnerChildren.reserve(Partner->Children.size());
        for (auto Child : Partner->Children)
          PartnerChildren.insert(Child);

        for (auto Child : OutOfOrderNodes) {
          auto ChildPartner = findMapping(NewToOldMapping, Child);
          if (PartnerChildren.contains(ChildPartner)) {
            auto Index = findPosition(Child);
            Script.emplace_back(
                edit_action::MoveTree{.Tree = ChildPartner->Original,
                                      .Parent = Partner->Original,
                                      .Index = Index});
            OldTree.move(ChildPartner, Partner, Index);
            ChildPartner->Marker = true;
            Child->Marker = true;
          }
        }
      }
    }
  }

  for (auto Node : VirtualTree::makePostOrder(OldTree.getRoot()))
    if (!findMapping(OldToNewMapping, Node))
      Script.emplace_back(edit_action::DeleteNode{.Leaf = Node->Original});

  return Script;
}

std::size_t BaseTreeDiff::findPosition(VirtualTree::Node *Node) const {
  VirtualTree::Node *Predecessor{nullptr};

  for (const auto Child : Node->Parent->Children) {
    if (Child == Node)
      break;
    if (Child->Marker)
      Predecessor = Child;
  }
  if (!Predecessor)
    return 0;

  auto OldPredecessor = findMapping(NewToOldMapping, Predecessor);
  std::size_t Index = 0;

  for (const auto Child : OldPredecessor->Parent->Children) {
    if (Child == OldPredecessor)
      break;
    if (Child->Marker)
      ++Index;
  }
  return Index + 1;
}

EditScript BaseTreeDiff::diff(const MerkleTree &Old, const MerkleTree &New) {
  OldTree.build(Old.getRoot());
  NewTree.build(New.getRoot());
  OldToNewMapping.reserve(Old.getRoot().getSize());
  NewToOldMapping.reserve(New.getRoot().getSize());

  match(OldTree.getRoot()->Children, NewTree.getRoot()->Children);
  return makeEditScript();
}

EditScript BaseTreeDiff::diffink(const MerkleTree &Old, const MerkleTree &New) {
  OldTree.setRoot(OldTree.pushBack(Old.getRoot(), nullptr));
  NewTree.setRoot(NewTree.pushBack(New.getRoot(), nullptr));
  OldToNewMapping.reserve(Old.getRoot().getSize());
  NewToOldMapping.reserve(New.getRoot().getSize());
  VirtualMap.reserve(New.getMapping().size() * 2);

  build(Old, OldTree, ChangedOldNodes, Old.getRoot(), OldTree.getRoot());
  build(New, NewTree, ChangedNewNodes, New.getRoot(), NewTree.getRoot());

  {
    std::vector<std::unordered_set<VirtualTree::Node *>> CrossMappingCandidates(
        New.getRoot().getHeight() + 1);

    insertMapping(OldTree.getRoot(), NewTree.getRoot());
    for (const auto [OldNode, NewNode] : New.getMapping()) {
      auto OldNodeCopy = VirtualMap[OldNode];
      auto NewNodeCopy = VirtualMap[NewNode];
      insertMapping(OldNodeCopy, NewNodeCopy);

      if (OldNode->isLeaf() && NewNode->isLeaf() &&
          !HashNode::isExactlyEqual(*OldNode, *NewNode))
        if (VirtualTree::Node::isEqual(NewNodeCopy->Parent,
                                       OldNodeCopy->Parent))
          CrossMappingCandidates[NewNodeCopy->Parent->VirtualHeight].insert(
              NewNodeCopy->Parent);
    }

    for (auto Node : ChangedNewNodes)
      if (VirtualTree::Node::isEqual(
              Node->Parent, findMapping(NewToOldMapping, Node->Parent)))
        CrossMappingCandidates[Node->Parent->VirtualHeight].insert(
            Node->Parent);

    auto countCommons =
        [](const std::vector<VirtualTree::Node *> &Lhs,
           const std::vector<VirtualTree::Node *> &Rhs) -> std::size_t {
      if (Lhs.size() != Rhs.size())
        throw std::runtime_error("xxHash colision");
      std::size_t Count{0};
      for (std::size_t i{0}; i != Lhs.size(); ++i)
        if (HashNode::isExactlyEqual(Lhs[i]->Original, Rhs[i]->Original))
          ++Count;
      return Count;
    };

    for (std::size_t Height{2}; Height != CrossMappingCandidates.size();
         ++Height)
      for (auto Node : CrossMappingCandidates[Height])
        if (Node->Parent &&
            VirtualTree::Node::isEqual(
                Node->Parent, findMapping(NewToOldMapping, Node->Parent)))
          CrossMappingCandidates[Node->Parent->VirtualHeight].insert(
              Node->Parent);

    for (auto Candidates = CrossMappingCandidates.crbegin();
         Candidates != CrossMappingCandidates.crend(); ++Candidates) {
      if (Candidates->size() < 2)
        continue;

      for (auto Iter = Candidates->cbegin(); Iter != Candidates->cend();
           ++Iter) {
        auto Descendants = VirtualTree::makePostOrder(*Iter);
        auto Partner = findMapping(NewToOldMapping, *Iter);
        auto PartnerDescendants = VirtualTree::makePostOrder(Partner);

        auto Max = countCommons(Descendants, PartnerDescendants);
        auto &MaxDescendants = Descendants;

        for (auto AltIter = std::next(Iter); AltIter != Candidates->cend();
             ++AltIter)
          if (VirtualTree::Node::isEqual(*Iter, *AltIter)) {
            auto AltDescendants = VirtualTree::makePostOrder(*AltIter);
            auto AltMax = countCommons(AltDescendants, PartnerDescendants);
            if (AltMax > Max) {
              Max = AltMax;
              MaxDescendants = std::move(AltDescendants);
            }
          }

        if (&Descendants != &MaxDescendants)
          for (std::size_t i{0}; i != Descendants.size(); ++i)
            swapMapping(Descendants[i], MaxDescendants[i]);
      }
    }
  }

  for (auto Node : ChangedOldNodes)
    OldTree.buildSubtree(Node);
  for (auto Node : ChangedNewNodes)
    NewTree.buildSubtree(Node);
  match(ChangedOldNodes, ChangedNewNodes);
  return makeEditScript();
}

} // namespace diffink