#ifndef DIFFINK_TREEDIFF_TREEDIFF_H
#define DIFFINK_TREEDIFF_TREEDIFF_H

#include "DiffInk/AST/MerkleTree.h"
#include "DiffInk/AST/VirtualTree.h"
#include "DiffInk/TreeDiff/EditAction.h"
#include <forward_list>
#include <queue>

namespace diffink {

class TreeDiff {
public:
  class Matcher {
  public:
    virtual ~Matcher() = default;
    virtual void match(TreeDiff &Mapping, const std::vector<VirtualNode *> &Old,
                       const std::vector<VirtualNode *> &New) = 0;
  };

private:
  static constexpr XXH64_hash_t UncommonSymbolHash{0};

  VirtualTree OldTree;
  VirtualTree NewTree;
  std::unordered_map<VirtualNode *, VirtualNode *> OldToNewMapping;
  std::unordered_map<VirtualNode *, VirtualNode *> NewToOldMapping;

  std::unordered_map<const HashNode *, VirtualNode *> VirtualMap;
  std::vector<VirtualNode *> UncommonOldNodes;
  std::vector<VirtualNode *> UncommonNewNodes;

private:
  TreeDiff() noexcept = default;

  ~TreeDiff() = default;

  void match(Matcher *Mat, const MerkleTree &Old, const MerkleTree &New);

  void matchDiffInk(Matcher *Mat, const MerkleTree &Old, const MerkleTree &New);

  /*                        *\
   * DiffInk core algorithm *
  \*                        */
  void earlyMatching(const MerkleTree &New);

  std::size_t countCommons(VirtualNode *Left, VirtualNode *Right) const;

  void build(const MerkleTree &OriginalTree, VirtualTree &TreeCopy,
             std::vector<VirtualNode *> &ChangeSet,
             const HashNode &OriginalNode, VirtualNode *NodeCopy);

  EditScript makeEditScript();

  void swapMapping(VirtualNode *NewNode, VirtualNode *AltNewNode);

  std::size_t findPosition(VirtualNode *Node) const;

public:
  VirtualNode *findOldToNewMapping(VirtualNode *Node) const;

  VirtualNode *findNewToOldMapping(VirtualNode *Node) const;

  VirtualNode *getOldRoot() const noexcept { return OldTree.getRoot(); }

  VirtualNode *getNewRoot() const noexcept { return NewTree.getRoot(); }

  bool areContained(VirtualNode *OldNode, VirtualNode *NewNode) const;

  void insertMapping(VirtualNode *OldNode, VirtualNode *NewNode);

  void overrideMapping(VirtualNode *OldNode, VirtualNode *NewNode);

  static EditScript runDiff(Matcher *Mat, const MerkleTree &Old,
                            const MerkleTree &New);
};

} // namespace diffink

#endif // DIFFINK_TREEDIFF_TREEDIFF_H