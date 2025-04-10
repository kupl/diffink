#ifndef AST_HASHNODE_H
#define AST_HASHNODE_H

#include <optional>
#include <string>
#include <tree_sitter/api.h>
#include <vector>
#include <xxhash.hpp>

namespace diffink {

class HashNode {
private:
  static constexpr std::size_t BitMode{128};
  xxh::hash_t<BitMode> ExactHash;
  xxh::hash_t<BitMode> StructuralHash;
  std::size_t Height;

  HashNode *Parent;
  std::vector<HashNode *> Children;
  TSSymbol Type;
  std::string Text;

public:
  HashNode(HashNode *Parent, TSSymbol Type, const std::string &Text)
      : Height{0}, Parent{Parent}, Type{Type}, Text(Text) {
    Parent->Children.push_back(this);
  }

  bool isLeaf() const { return Height == 1; }

  std::size_t getHeight() const { return Height; }

  TSSymbol getType() const { return Type; }

  const std::string &getText() const { return Text; }

  void setMetadataRecursively();
};

} // namespace diffink

#endif // AST_HASHNODE_H