#ifndef TREEDIFF_BASETREEDIFF_H
#define TREEDIFF_BASETREEDIFF_H

#include "AST/MerkleTree.h"
#include "AST/VirtualTree.h"
#include "TreeDiff/EditAction.h"
#include <forward_list>
#include <queue>

namespace diffink {

class BaseTreeDiff {
private:
  static constexpr std::size_t BitMode = HashNode::BitMode;

private:
  VirtualTree OldTree;
  VirtualTree NewTree;
  std::unordered_map<VirtualTree::Node *, VirtualTree::Node *> OldToNewMapping;
  std::unordered_map<VirtualTree::Node *, VirtualTree::Node *> NewToOldMapping;

  std::unordered_map<const HashNode *, VirtualTree::Node *> VirtualMap;
  std::vector<VirtualTree::Node *> ChangedOldNodes;
  std::vector<VirtualTree::Node *> ChangedNewNodes;

  const TSSymbol ChangedSymbol{static_cast<TSSymbol>(-1)};
  const xxh::hash_t<BitMode> ChangedSymbolHash;

private:
  void build(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
             std::vector<VirtualTree::Node *> &ChangeSet,
             const HashNode &OriginalNode, VirtualTree::Node *NodeCopy);

  void swapMapping(VirtualTree::Node *NewNode, VirtualTree::Node *AltNewNode);

  VirtualTree::Node *
  findMapping(const std::unordered_map<VirtualTree::Node *, VirtualTree::Node *>
                  &Mapping,
              VirtualTree::Node *Node) const;

  bool areContained(VirtualTree::Node *OldNode,
                    VirtualTree::Node *NewNode) const;

  EditScript makeEditScript();

  std::size_t findPosition(VirtualTree::Node *Node) const;

protected:
  void insertMapping(VirtualTree::Node *OldNode, VirtualTree::Node *NewNode);

  virtual void match(const std::vector<VirtualTree::Node *> &Old,
                     const std::vector<VirtualTree::Node *> &New) {}

public:
  BaseTreeDiff() noexcept
      : ChangedSymbolHash{
            xxh::xxhash3<BitMode>(&ChangedSymbol, sizeof(ChangedSymbol))} {}

  ~BaseTreeDiff() = default;

  EditScript diff(const MerkleTree &Old, const MerkleTree &New);

  EditScript diffink(const MerkleTree &Old, const MerkleTree &New);
};

} // namespace diffink

#endif // TREEDIFF_BASETREEDIFF_H