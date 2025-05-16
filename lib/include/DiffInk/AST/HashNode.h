#ifndef DIFFINK_AST_HASHNODE_H
#define DIFFINK_AST_HASHNODE_H

#include "DiffInk/AST/SourceCode.h"
#include "DiffInk/Utils/XxhStl.h"
#include <format>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace diffink {

class HashNode {
public:
  static constexpr std::size_t DefaultIndent{2};

  struct UTF8Range {
    TSPoint StartPos;
    TSPoint EndPos;

    std::string toString() const;
  };

  // struct BuildConfig {
  //   std::unordered_set<std::string> Flattened;
  //   std::unordered_set<std::string> Aliased;
  //   std::unordered_set<std::string> Ignored;
  // };

private:
  const std::string Type;
  const std::string Label;
  std::list<HashNode> Children;
  const std::pair<uint32_t, uint32_t> ByteRange;
  const UTF8Range PosRange;

  uint32_t Height{0};
  uint32_t Size{1};
  XXH64_hash_t ExactHash;
  XXH64_hash_t StructuralHash;

private:
  void toStringRecursively(std::string &Buffer, std::size_t Depth,
                           std::size_t Indent) const;

  // static void build(const SourceCode &Code, TSTreeCursor &Cursor,
  //                   HashNode &Parent, const BuildConfig &Config);

  static void build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent);

  // Metadata = {Height, Size, ExactHash}
  void makeMetadataRecursively();

  void makePostOrder(std::vector<const HashNode *> &PostOrder,
                     const HashNode *Iter) const;

public:
  HashNode(const TSNode &RawNode, std::string &&Type, const SourceCode &Code,
           bool IsLeaf) noexcept;

  void makeStructuralHashRecursively();

  bool isLeaf() const noexcept { return Height == 0; }

  const std::string &getType() const noexcept { return Type; }

  const std::string &getLabel() const noexcept { return Label; }

  const decltype(Children) &getChildren() const noexcept { return Children; }

  const decltype(ByteRange) &getByteRange() const noexcept { return ByteRange; }

  uint32_t getHeight() const noexcept { return Height; }

  uint32_t getSize() const noexcept { return Size; }

  XXH64_hash_t getTypeHash() const noexcept { return xxhString64(Type); }

  XXH64_hash_t getStructuralHash() const noexcept { return StructuralHash; }

  std::vector<const HashNode *> makePostOrder() const;

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  // static std::unique_ptr<HashNode>
  // build(TSNode RootNode, const SourceCode &Code, const BuildConfig &Config);

  static std::unique_ptr<HashNode> build(TSNode RootNode,
                                         const SourceCode &Code);

  static bool eqaulExactly(const HashNode &Left,
                           const HashNode &Right) noexcept {
    return Left.ExactHash == Right.ExactHash;
  }

  static bool equalStructurally(const HashNode &Left,
                                const HashNode &Right) noexcept {
    return Left.StructuralHash == Right.StructuralHash;
  }
};

} // namespace diffink

#endif // DIFFINK_AST_HASHNODE_H