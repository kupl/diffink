#ifndef AST_HASHNODE_H
#define AST_HASHNODE_H

#include "AST/SourceCode.h"
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

private:
  static constexpr std::size_t DefaultIndent{2};

  const TSSymbol Symbol;
  const std::string Type;
  const std::string Value;
  std::list<HashNode> Children;
  const std::pair<std::string::size_type, std::string::size_type> ByteRange;
  const UTF8Range PosRange;

  std::size_t Height{0};
  std::size_t Size{0};
  xxh::hash_t<BitMode> SymbolHash;
  xxh::hash_t<BitMode> ExactHash;
  xxh::hash_t<BitMode> StructuralHash;

private:
  void toStringRecursively(std::string &Buffer, std::size_t CurrentIndentSize,
                           std::size_t Indent) const;

  // Return false if the tree has an error node
  static bool build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent);

  // Metadata = {Height, Size, ExactHash}
  void makeMetadataRecursively();

public:
  HashNode(const TSNode &RawNode, const SourceCode &Code, bool IsLeaf) noexcept;

  HashNode(const HashNode &Rhs) = delete;

  void makeStructuralHashRecursively();

  bool isLeaf() const noexcept { return Height == 1; }

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

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  // Return nullptr if the tree is empty or has an error node
  static std::unique_ptr<HashNode> build(TSNode RootNode,
                                         const SourceCode &Code);

  static bool isExactlyEqual(const HashNode &Lhs,
                             const HashNode &Rhs) noexcept {
    return Lhs.ExactHash.high64 == Rhs.ExactHash.high64 &&
           Lhs.ExactHash.low64 == Rhs.ExactHash.low64;
  }

  static bool isStructurallyEqual(const HashNode &Lhs,
                                  const HashNode &Rhs) noexcept {
    return Lhs.StructuralHash.high64 == Rhs.StructuralHash.high64 &&
           Lhs.StructuralHash.low64 == Rhs.StructuralHash.low64;
  }
};

} // namespace diffink

#endif // AST_HASHNODE_H