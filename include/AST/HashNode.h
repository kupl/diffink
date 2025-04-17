#ifndef AST_HASHNODE_H
#define AST_HASHNODE_H

#include "AST/SourceCode.h"
#include <list>
#include <memory>
#include <xxhash.hpp>

namespace diffink {

class HashNode {
public:
  struct UTF8Range {
    TSPoint StartPos;
    TSPoint EndPos;

    std::string toString() const;
  };

private:
  static constexpr std::size_t BitMode{128};
  static constexpr std::size_t DefaultIndent{2};

  const TSSymbol Symbol;
  const std::string Type;
  const std::string Value;
  std::list<HashNode> Children;
  const std::pair<std::string::size_type, std::string::size_type> ByteRange;
  const UTF8Range PosRange;

  std::size_t Height{0};
  xxh::hash_t<BitMode> ExactHash;
  xxh::hash_t<BitMode> StructuralHash;

private:
  void toStringRecursively(std::string &Buffer, std::size_t CurrentIndentSize,
                           std::size_t Indent) const;

  // Return false if the tree has an error node
  static bool build(const SourceCode &Code, TSTreeCursor &Cursor,
                    HashNode &Parent);

  // Metadata = {Height, ExactHash}
  void makeMetadataRecursively();

public:
  HashNode(const TSNode &RawNode, const SourceCode &Code, bool IsLeaf) noexcept;

  HashNode(const HashNode &Rhs) = delete;

  void makeStructuralHashRecursively();

  bool isLeaf() const noexcept { return Children.empty(); }

  TSSymbol getSymbol() const noexcept { return Symbol; }

  const std::string &getType() const noexcept { return Type; }

  const std::string &getTextValue() const noexcept { return Value; }

  const decltype(Children) &getChildren() const noexcept { return Children; }

  std::size_t getHeight() const noexcept { return Height; }

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  // Return nullptr if the tree is empty or has an error node
  static std::unique_ptr<HashNode> build(TSNode RootNode,
                                         const SourceCode &Code);

  static bool isExactlyEqual(const HashNode &Lhs, const HashNode &Rhs) noexcept;

  static bool isStructurallyEqual(const HashNode &Lhs,
                                  const HashNode &Rhs) noexcept;
};

} // namespace diffink

#endif // AST_HASHNODE_H