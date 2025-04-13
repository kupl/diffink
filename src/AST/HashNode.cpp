#include "AST/HashNode.h"

namespace diffink {

HashNode::HashNode(RawNode &&Info, HashNode *Parent)
    : Parent{Parent}, Height{0} {
  if (Parent)
    Parent->Children.push_back(this);
  this->Info = std::move(Info);
  Index = Parent ? Parent->Children.size() - 1 : 0;
}

void HashNode::toStringRecursively(std::string &Buffer,
                                   std::size_t CurrentIndentSize,
                                   std::size_t Indent) const {
  Buffer.append(' ', CurrentIndentSize).append(toString());
  for (auto Child : Children)
    toStringRecursively(Buffer, CurrentIndentSize + Indent, Indent);
}

std::string HashNode::toString() const {
  std::string Buffer;
  Buffer.append(Info.Type);
  if (!Info.Text.empty())
    Buffer.append(" : \"").append(Info.Text).push_back('"');
  Buffer.append(" [")
      .append(std::to_string(Info.StartByte))
      .append(",")
      .append(std::to_string(Info.EndByte))
      .append("]\n");
  return Buffer;
}

std::string HashNode::toStringRecursively(std::size_t Indent) const {
  std::string Buffer;
  toStringRecursively(Buffer, 0, Indent);
  return Buffer;
}

void HashNode::setMetadataRecursively() {
  auto SymbolHash = xxh::xxhash3<BitMode>(&Info.Symbol, sizeof(Info.Symbol));
  if (isLeaf()) {
    ExactHash =
        xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(Info.Text)});
    StructuralHash = SymbolHash;
    Height = 1;
    return;
  }

  std::vector<xxh::hash_t<BitMode>> StructuralHashList, ExactHashList;
  ExactHashList.reserve(Children.size());
  StructuralHashList.reserve(Children.size());

  for (auto Child : Children) {
    Child->setMetadataRecursively();
    ExactHashList.push_back(Child->ExactHash);
    StructuralHashList.push_back(Child->StructuralHash);
    if (Child->Height >= Height)
      Height = Child->Height + 1;
  }

  ExactHash =
      xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(ExactHashList)});
  StructuralHash = xxh::xxhash3<BitMode>(
      {SymbolHash, xxh::xxhash3<BitMode>(StructuralHashList)});
}

} // namespace diffink