#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink {

void TreeDiff::match(Matcher *Mat, const MerkleTree &Old,
                     const MerkleTree &New) {
  OldTree.build(Old.getRoot());
  NewTree.build(New.getRoot());
  Mat->match(*this, {OldTree.getRoot()}, {NewTree.getRoot()});
}

void TreeDiff::matchDiffInk(Matcher *Mat, const MerkleTree &Old,
                            const MerkleTree &New) {
  OldTree.setRoot(OldTree.pushBack(Old.getRoot(), nullptr));
  NewTree.setRoot(NewTree.pushBack(New.getRoot(), nullptr));
  VirtualMap.reserve(New.getMapping().size() * 2);

  build(Old, OldTree, UncommonOldNodes, Old.getRoot(), OldTree.getRoot());
  build(New, NewTree, UncommonNewNodes, New.getRoot(), NewTree.getRoot());

  earlyMatching(New);
  for (auto Node : UncommonOldNodes)
    OldTree.buildSubtree(Node);
  for (auto Node : UncommonNewNodes)
    NewTree.buildSubtree(Node);
  if (!UncommonOldNodes.empty() && !UncommonNewNodes.empty())
    Mat->match(*this, UncommonOldNodes, UncommonNewNodes);
}

void TreeDiff::earlyMatching(const MerkleTree &New) {
  std::vector<std::unordered_set<VirtualNode *>> CrossMappingCandidates(
      New.getRoot().getHeight() + 1);

  insertMapping(OldTree.getRoot(), NewTree.getRoot());
  for (const auto [OldNode, NewNode] : New.getMapping()) {
    auto OldNodeCopy = VirtualMap[OldNode];
    auto NewNodeCopy = VirtualMap[NewNode];
    insertMapping(OldNodeCopy, NewNodeCopy);
    if (OldNode->isLeaf() && NewNode->isLeaf() &&
        !HashNode::eqaulExactly(*OldNode, *NewNode) &&
        VirtualNode::eqaul(NewNodeCopy->Parent, OldNodeCopy->Parent))
      CrossMappingCandidates[NewNodeCopy->Parent->VirtualHeight].insert(
          NewNodeCopy->Parent);
  }

  for (auto Node : UncommonNewNodes)
    if (VirtualNode::eqaul(Node->Parent, findNewToOldMapping(Node->Parent)))
      CrossMappingCandidates[Node->Parent->VirtualHeight].insert(Node->Parent);

  for (std::size_t Height{1}; Height != CrossMappingCandidates.size(); ++Height)
    for (auto Node : CrossMappingCandidates[Height])
      if (Node->Parent &&
          VirtualNode::eqaul(Node->Parent, findNewToOldMapping(Node->Parent)))
        CrossMappingCandidates[Node->Parent->VirtualHeight].insert(
            Node->Parent);

  for (auto Candidates = CrossMappingCandidates.crbegin();
       Candidates != CrossMappingCandidates.crend(); ++Candidates) {
    if (Candidates->size() < 2)
      continue;

    for (auto Iter = Candidates->cbegin(); Iter != Candidates->cend(); ++Iter) {
      auto Node = *Iter;
      auto Partner = findNewToOldMapping(Node);
      auto Index = Node;
      auto Max = countCommons(Node, Partner);

      for (auto AltIter = std::next(Iter); AltIter != Candidates->cend();
           ++AltIter)
        if (VirtualNode::eqaul(Node, *AltIter)) {
          auto AltMax = countCommons(*AltIter, Partner);
          if (AltMax > Max) {
            Max = AltMax;
            Index = *AltIter;
          }
        }

      if (Index != Node)
        VirtualNode::traversePostOrder(
            Node, Index, [this](VirtualNode *Left, VirtualNode *Right) {
              swapMapping(Left, Right);
            });
    }
  }

  for (auto Node : UncommonNewNodes) {
    auto ParentPartner = findNewToOldMapping(Node->Parent);
    std::size_t Index{0};
    for (; Index != ParentPartner->Children.size(); ++Index)
      if (Node->Parent->Children[Index] == Node)
        break;

    if (Index < ParentPartner->Children.size()) {
      auto Partner = ParentPartner->Children[Index];
      if (HashNode::eqaulExactly(Partner->Original, Node->Original))
        insertMapping(Partner, Node);
    }
  }
}

std::size_t TreeDiff::countCommons(VirtualNode *Left,
                                   VirtualNode *Right) const {
  std::size_t Count{0};
  VirtualNode::traversePostOrder(
      Left, Right, [&Count](VirtualNode *Left, VirtualNode *Right) {
        if (HashNode::eqaulExactly(Left->Original, Right->Original))
          ++Count;
      });
  return Count;
}

void TreeDiff::build(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
                     std::vector<VirtualNode *> &ChangeSet,
                     const HashNode &OriginalNode, VirtualNode *NodeCopy) {
  NodeCopy->VirtualHeight = 0;
  if (OriginalNode.getChildren().empty()) {
    NodeCopy->VirtualHash = OriginalNode.getTypeHash();
    return;
  }
  NodeCopy->VirtualHeight = 1;

  std::vector<XXH128_hash_t> Hashes;
  Hashes.reserve(OriginalNode.getChildren().size());

  for (auto &Child : OriginalNode.getChildren()) {
    auto Temp = TreeCopy.pushBack(Child, NodeCopy);
    if (OriginalTree.isUncommon(Child)) {
      Temp->VirtualHash = UncommonSymbolHash;
      ChangeSet.push_back(Temp);
      Hashes.push_back(UncommonSymbolHash);
    }

    else {
      VirtualMap.emplace(&Child, Temp);
      if (OriginalTree.hasUncommonChild(Child)) {
        build(OriginalTree, TreeCopy, ChangeSet, Child, Temp);
        Hashes.push_back(Temp->VirtualHash);
        NodeCopy->VirtualHeight =
            std::max(NodeCopy->VirtualHeight, Temp->VirtualHeight + 1);
      } else
        Hashes.push_back(Child.getStructuralHash());
    }
  }

  NodeCopy->VirtualHash =
      xxhVector({OriginalNode.getTypeHash(), xxhVector(Hashes)});
}

EditScript TreeDiff::makeEditScript() {
  EditScript Script;
  std::queue<VirtualNode *> Queue;
  Queue.push(NewTree.getRoot());

  while (!Queue.empty()) {
    auto Node = Queue.front();
    Queue.pop();
    for (auto Child : Node->Children)
      Queue.push(Child);
    auto Partner = findNewToOldMapping(Node);
    auto ParentPartner = findNewToOldMapping(Node->Parent);

    if (!Partner) {
      auto Index = findPosition(Node);
      Script.emplace_back(
          edit_action::InsertNode{.Leaf = Node->Original,
                                  .Parent = Node->Parent->Original,
                                  .Index = Index});
      Partner = OldTree.insert(Node->Original, ParentPartner, Index);
      insertMapping(Partner, Node);
    }

    else if (Node->Parent) {
      if (Partner->Original.getLabel() != Node->Original.getLabel())
        Script.emplace_back(edit_action::UpdateNode{
            .Leaf = Partner->Original, .UpdatedLeaf = Node->Original});

      if (ParentPartner != Partner->Parent) {
        auto Index = findPosition(Node);
        Script.emplace_back(
            edit_action::MoveTree{.Subtree = Partner->Original,
                                  .Parent = ParentPartner->Original,
                                  .Index = Index,
                                  .MovedSubtree = Node->Original});
        OldTree.move(Partner, ParentPartner, Index);
      }
    }

    std::vector<std::vector<std::size_t>> LcsTable(
        Partner->Children.size() + 1,
        std::vector<std::size_t>(Node->Children.size() + 1, 0));

    for (std::size_t i{1}; i != Partner->Children.size() + 1; ++i)
      for (std::size_t j{1}; j != Node->Children.size() + 1; ++j)
        LcsTable[i][j] =
            areContained(Partner->Children[i - 1], Node->Children[j - 1])
                ? LcsTable[i - 1][j - 1] + 1
                : std::max(LcsTable[i - 1][j], LcsTable[i][j - 1]);

    std::forward_list<VirtualNode *> OutOfOrderNodes;

    for (std::size_t i = Partner->Children.size(), j = Node->Children.size();
         i != 0 && j != 0;) {
      if (LcsTable[i][j] == LcsTable[i - 1][j - 1] + 1 &&
          areContained(Partner->Children[i - 1], Node->Children[j - 1])) {
        --i;
        --j;
        Partner->Children[i]->Marker = true;
        Node->Children[j]->Marker = true;
        continue;
      }

      else if (LcsTable[i][j] == LcsTable[i][j - 1]) {
        --j;
        OutOfOrderNodes.push_front(Node->Children[j]);
      } else
        --i;
    }

    if (!OutOfOrderNodes.empty()) {
      std::unordered_set<VirtualNode *> PartnerChildren;
      PartnerChildren.reserve(Partner->Children.size());
      for (auto Child : Partner->Children)
        PartnerChildren.insert(Child);

      for (auto Child : OutOfOrderNodes) {
        auto ChildPartner = findNewToOldMapping(Child);
        if (PartnerChildren.contains(ChildPartner)) {
          auto Index = findPosition(Child);
          Script.emplace_back(
              edit_action::MoveTree{.Subtree = ChildPartner->Original,
                                    .Parent = Partner->Original,
                                    .Index = Index,
                                    .MovedSubtree = Child->Original});
          OldTree.move(ChildPartner, Partner, Index);
          ChildPartner->Marker = true;
          Child->Marker = true;
        }
      }
    }
  }

  VirtualNode::traversePostOrder(
      OldTree.getRoot(), [&Script, this](VirtualNode *Node) {
        if (!findOldToNewMapping(Node))
          Script.emplace_back(edit_action::DeleteNode{
              .Leaf = Node->Original, .Parent = Node->Parent->Original});
      });
  return Script;
}

void TreeDiff::swapMapping(VirtualNode *NewNode, VirtualNode *AltNewNode) {
  if (auto OldNode = findNewToOldMapping(NewNode); OldNode)
    if (auto AltOldNode = findNewToOldMapping(AltNewNode); AltOldNode) {
      NewToOldMapping[NewNode] = AltOldNode;
      NewToOldMapping[AltNewNode] = OldNode;
      OldToNewMapping[OldNode] = AltNewNode;
      OldToNewMapping[AltOldNode] = NewNode;
    }
}

std::size_t TreeDiff::findPosition(VirtualNode *Node) const {
  VirtualNode *Predecessor{nullptr};

  for (const auto Child : Node->Parent->Children) {
    if (Child == Node)
      break;
    if (Child->Marker)
      Predecessor = Child;
  }
  if (!Predecessor)
    return 0;

  auto OldPredecessor = findNewToOldMapping(Predecessor);
  return OldPredecessor->Parent->findChild(OldPredecessor) + 1;
}

VirtualNode *TreeDiff::findOldToNewMapping(VirtualNode *Node) const {
  auto Iter = OldToNewMapping.find(Node);
  return Iter != OldToNewMapping.cend() ? Iter->second : nullptr;
}

VirtualNode *TreeDiff::findNewToOldMapping(VirtualNode *Node) const {
  auto Iter = NewToOldMapping.find(Node);
  return Iter != NewToOldMapping.cend() ? Iter->second : nullptr;
}

bool TreeDiff::areContained(VirtualNode *OldNode, VirtualNode *NewNode) const {
  auto Temp = findNewToOldMapping(NewNode);
  return Temp == OldNode;
}

void TreeDiff::insertMapping(VirtualNode *OldNode, VirtualNode *NewNode) {
  OldToNewMapping.emplace(OldNode, NewNode);
  NewToOldMapping.emplace(NewNode, OldNode);
}

void TreeDiff::overrideMapping(VirtualNode *OldNode, VirtualNode *NewNode) {
  auto AltOld = findNewToOldMapping(NewNode);
  auto AltNew = findOldToNewMapping(OldNode);
  if (AltOld && AltOld != OldNode)
    OldToNewMapping.erase(AltOld);
  if (AltNew && AltNew != NewNode)
    NewToOldMapping.erase(AltNew);

  OldToNewMapping[OldNode] = NewNode;
  NewToOldMapping[NewNode] = OldNode;
}

EditScript TreeDiff::runDiff(Matcher *Mat, const MerkleTree &Old,
                             const MerkleTree &New) {
  TreeDiff Mapping;
  Mapping.match(Mat, Old, New);
  return Mapping.makeEditScript();
}

EditScript TreeDiff::runDiffInk(Matcher *Mat, const MerkleTree &Old,
                                const MerkleTree &New) {
  TreeDiff Mapping;
  Mapping.matchDiffInk(Mat, Old, New);
  return Mapping.makeEditScript();
}

} // namespace diffink