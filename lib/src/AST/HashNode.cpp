#include "DiffInk/AST/HashNode.h"

namespace diffink {

void HashNode::toStringRecursively(std::string &Buffer, std::size_t Depth,
                                   std::size_t Indent) const {
  Buffer.append(Depth * Indent, ' ').append(toString()).push_back('\n');
  for (auto &Child : Children)
    Child.toStringRecursively(Buffer, Depth + 1, Indent);
}

std::string HashNode::UTF8Range::toString() const {
  return std::format("({},{})-({},{})", StartPos.row + 1, StartPos.column + 1,
                     EndPos.row + 1, EndPos.column + 1);
}

bool HashNode::build(const SourceCode &Code, TSTreeCursor &Cursor,
                     HashNode &Parent, const BuildConfig &Config) {
  auto Node = ts_tree_cursor_current_node(&Cursor);
  if (ts_node_is_error(Node))
    return false;

  std::string NodeType{ts_node_type(Node)};
  if (Config.Ignored.contains(NodeType))
    return true;
  bool IsFlattened = Config.Flattened.contains(NodeType);
  auto AliasIter = Config.Aliased.find(NodeType);
  if (AliasIter != Config.Aliased.cend())
    NodeType = *AliasIter;

  if (!ts_tree_cursor_goto_first_child(&Cursor))
    Parent.Children.emplace_back(Node, std::move(NodeType), Code, true);
  else {
    auto &CurNode =
        Parent.Children.emplace_back(Node, std::move(NodeType), Code, false);
    if (!IsFlattened)
      do
        if (!build(Code, Cursor, CurNode, Config))
          return false;
      while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
  return true;
}

HashNode::HashNode(const TSNode &RawNode, std::string &&Type,
                   const SourceCode &Code, bool IsLeaf) noexcept
    : Type(std::move(Type)),
      ByteRange{ts_node_start_byte(RawNode), ts_node_end_byte(RawNode)},
      Label(IsLeaf ? Code.getSubstring(ts_node_start_byte(RawNode),
                                       ts_node_end_byte(RawNode))
                   : ""),
      PosRange(UTF8Range{Code.getUTF8Position(ts_node_start_byte(RawNode)),
                         Code.getUTF8Position(ts_node_end_byte(RawNode))}) {}

void HashNode::makeMetadataRecursively() {
  TypeHash = xxhString(Type);
  if (Children.empty()) {
    ExactHash = xxhVector({TypeHash, xxhString(Label)});
    return;
  }

  std::vector<XXH128_hash_t> ExactHashes;
  ExactHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeMetadataRecursively();
    ExactHashes.push_back(Child.ExactHash);
    Height = std::max(Height, Child.Height + 1);
    Size += Child.Size;
  }
  ExactHash = xxhVector({TypeHash, xxhVector(ExactHashes)});
}

void HashNode::makePostOrder(std::vector<const HashNode *> &PostOrder,
                             const HashNode *Iter) const {
  for (const auto &Child : Iter->Children)
    makePostOrder(PostOrder, &Child);
  PostOrder.push_back(Iter);
}

void HashNode::makeStructuralHashRecursively() {
  if (isLeaf()) {
    StructuralHash = TypeHash;
    return;
  }

  std::vector<XXH128_hash_t> StructuralHashes;
  StructuralHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeStructuralHashRecursively();
    StructuralHashes.push_back(Child.StructuralHash);
  }
  StructuralHash = xxhVector({TypeHash, xxhVector(StructuralHashes)});
}

std::vector<const HashNode *> HashNode::makePostOrder() const {
  std::vector<const HashNode *> PostOrder;
  PostOrder.reserve(Size);
  makePostOrder(PostOrder, this);
  return PostOrder;
}

std::string HashNode::toString() const {
  if (Label.empty())
    return std::format("{} {}", Type, PosRange.toString());
  return std::format("{}: '{}' {}", Type, Label, PosRange.toString());
}

std::string HashNode::toStringRecursively(std::size_t Indent) const {
  std::string Buffer;
  toStringRecursively(Buffer, 0, Indent);
  Buffer.pop_back();
  return Buffer;
}

std::unique_ptr<HashNode> HashNode::build(TSNode RootNode,
                                          const SourceCode &Code,
                                          const BuildConfig &Config) {
  if (ts_node_is_null(RootNode) || ts_node_is_error(RootNode))
    return nullptr;

  auto Root =
      std::make_unique<HashNode>(RootNode, ts_node_type(RootNode), Code, false);
  auto Cursor = ts_tree_cursor_new(RootNode);

  if (ts_tree_cursor_goto_first_child(&Cursor))
    do
      if (!build(Code, Cursor, *Root, Config))
        return nullptr;
    while (ts_tree_cursor_goto_next_sibling(&Cursor));

  ts_tree_cursor_delete(&Cursor);
  Root->makeMetadataRecursively();
  return Root;
}

XXH128_hash_t xxhVector(const std::vector<XXH128_hash_t> &data) {
  return XXH128(static_cast<const void *>(data.data()),
                data.size() * sizeof(XXH128_hash_t), 0);
}

XXH128_hash_t xxhString(const std::string &data) {
  return XXH128(static_cast<const void *>(data.data()), data.size(), 0);
}

} // namespace diffink