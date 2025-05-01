#ifndef DIFFINK_AST_HASHNODE_H
#define DIFFINK_AST_HASHNODE_H

#include "DiffInk/AST/SourceCode.h"
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <xxhash.hpp>

namespace diffink {

class HashNode {
public:
  static constexpr std::size_t BitMode{128};

  struct UTF8Range {
    TSPoint StartPos;
    TSPoint EndPos;

    std::string toString() const;
  };

  static constexpr std::size_t DefaultIndent{2};

private:
  const TSSymbol Symbol;
  const std::string Type;
  const std::string Value;
  std::list<HashNode> Children;
  const std::pair<std::string::size_type, std::string::size_type> ByteRange;
  const UTF8Range PosRange;

  std::size_t Height{0};
  std::size_t Size{1};
  xxh::hash_t<BitMode> SymbolHash;
  xxh::hash_t<BitMode> ExactHash;
  xxh::hash_t<BitMode> StructuralHash;

private:
  void toStringRecursively(std::string &Buffer, std::size_t Depth,
                           std::size_t Indent) const;

  // Return false if the tree has an error node
  static bool build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent, std::unordered_set<std::string> &Ignore);

  // Metadata = {Height, Size, ExactHash}
  void makeMetadataRecursively();

  void makePostOrder(std::vector<const HashNode *> &PostOrder,
                     const HashNode *Iter) const;

public:
  HashNode(const TSNode &RawNode, const SourceCode &Code, bool IsLeaf) noexcept;

  HashNode(const HashNode &Rhs) = delete;

  void makeStructuralHashRecursively();

  bool isLeaf() const noexcept { return Height == 0; }

  TSSymbol getSymbol() const noexcept { return Symbol; }

  const std::string &getType() const noexcept { return Type; }

  const std::string &getTextValue() const noexcept { return Value; }

  const decltype(Children) &getChildren() const noexcept { return Children; }

  const decltype(ByteRange) &getByteRange() const noexcept { return ByteRange; }

  std::size_t getHeight() const noexcept { return Height; }

  std::size_t getSize() const noexcept { return Size; }

  xxh::hash_t<BitMode> getSymbolHash() const noexcept { return SymbolHash; }

  xxh::hash_t<BitMode> getExactHash() const noexcept { return ExactHash; }

  xxh::hash_t<BitMode> getStructuralHash() const noexcept {
    return StructuralHash;
  }

  std::vector<const HashNode *> makePostOrder() const;

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  // Return nullptr if the tree is empty or has an error node
  static std::unique_ptr<HashNode>
  build(TSNode RootNode, const SourceCode &Code,
        std::unordered_set<std::string> &Ignore);

  static bool isExactlyEqual(const HashNode &Left,
                             const HashNode &Right) noexcept {
    return Left.ExactHash.high64 == Right.ExactHash.high64 &&
           Left.ExactHash.low64 == Right.ExactHash.low64;
  }

  static bool isStructurallyEqual(const HashNode &Left,
                                  const HashNode &Right) noexcept {
    return Left.StructuralHash.high64 == Right.StructuralHash.high64 &&
           Left.StructuralHash.low64 == Right.StructuralHash.low64;
  }
};

} // namespace diffink

#endif // DIFFINK_AST_HASHNODE_H