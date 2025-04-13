#ifndef AST_HASHNODE_H
#define AST_HASHNODE_H

#include "AST/RawNode.h"
#include <optional>
#include <vector>
#include <xxhash.hpp>

namespace diffink {

class HashNode {
public:
  struct Set {
    std::vector<const HashNode *> Subtrees;
  };

private:
  static constexpr std::size_t BitMode{128};
  static constexpr std::size_t DefaultIndent{2};

private:
  xxh::hash_t<BitMode> ExactHash;
  xxh::hash_t<BitMode> StructuralHash;

  RawNode Info;
  HashNode *Parent;
  std::vector<HashNode *> Children;
  std::size_t Height;
  std::size_t Index;

private:
  void toStringRecursively(std::string &Buffer, std::size_t CurrentIndentSize,
                           std::size_t Indent) const;

public:
  HashNode(RawNode &&Info, HashNode *Parent);

  bool isLeaf() const noexcept { return Height == 1; }

  const RawNode &getInfo() const noexcept { return Info; }

  const decltype(Children) &getChildren() const noexcept { return Children; }

  std::size_t getHeight() const noexcept { return Height; }

  std::string toString() const;

  std::string toStringRecursively(std::size_t Indent = DefaultIndent) const;

  void setMetadataRecursively();
};

} // namespace diffink

#endif // AST_HASHNODE_H