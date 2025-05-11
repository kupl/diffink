#ifndef DIFFINK_AST_HASHNODE_H
#define DIFFINK_AST_HASHNODE_H

#include "DiffInk/AST/SourceCode.h"
#include <format>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <xxHash/xxh3.h>

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
  const std::pair<std::string::size_type, std::string::size_type> ByteRange;
  const UTF8Range PosRange;

  std::size_t Height{0};
  std::size_t Size{1};
  XXH128_hash_t TypeHash;
  XXH128_hash_t ExactHash;
  XXH128_hash_t StructuralHash;

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

  std::size_t getHeight() const noexcept { return Height; }

  std::size_t getSize() const noexcept { return Size; }

  XXH128_hash_t getTypeHash() const noexcept { return TypeHash; }

  XXH128_hash_t getStructuralHash() const noexcept { return StructuralHash; }

  std::vector<const HashNode *> makePostOrder() const;

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  // static std::unique_ptr<HashNode>
  // build(TSNode RootNode, const SourceCode &Code, const BuildConfig &Config);

  static std::unique_ptr<HashNode> build(TSNode RootNode,
                                         const SourceCode &Code);

  static bool eqaulExactly(const HashNode &Left,
                           const HashNode &Right) noexcept {
    return Left.ExactHash.high64 == Right.ExactHash.high64 &&
           Left.ExactHash.low64 == Right.ExactHash.low64;
  }

  static bool equalStructurally(const HashNode &Left,
                                const HashNode &Right) noexcept {
    return Left.StructuralHash.high64 == Right.StructuralHash.high64 &&
           Left.StructuralHash.low64 == Right.StructuralHash.low64;
  }
};

XXH128_hash_t xxhVector(const std::vector<XXH128_hash_t> &data);

XXH128_hash_t xxhString(const std::string &data);

} // namespace diffink

#endif // DIFFINK_AST_HASHNODE_H