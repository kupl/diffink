#include "DiffInk/AST/HashNode.h"

namespace diffink {

std::string HashNode::UTF8Range::toString() const {
  return "(" + std::to_string(StartPos.row + 1) + "," +
         std::to_string(StartPos.column + 1) + ")-(" +
         std::to_string(EndPos.row + 1) + "," +
         std::to_string(EndPos.column + 1) + ")";
}

bool HashNode::build(const SourceCode &Code, TSTreeCursor &Cursor,
                     HashNode &Parent,
                     std::unordered_set<std::string> &Ignore) {
  auto Node = ts_tree_cursor_current_node(&Cursor);
  if (ts_node_is_error(Node))
    return false;
  if (Ignore.contains(ts_node_type(Node)))
    return true;

  if (!ts_tree_cursor_goto_first_child(&Cursor))
    Parent.Children.emplace_back(Node, Code, true);
  else {
    auto &HashNode = Parent.Children.emplace_back(Node, Code, false);
    do
      if (!build(Code, Cursor, HashNode, Ignore))
        return false;
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
  return true;
}

void HashNode::makeMetadataRecursively() {
  SymbolHash = xxh::xxhash3<BitMode>(&Symbol, sizeof(Symbol));

  if (Children.empty()) {
    ExactHash =
        xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(Value)});
    return;
  }

  std::vector<xxh::hash_t<BitMode>> ExactHashes;
  ExactHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeMetadataRecursively();
    ExactHashes.push_back(Child.ExactHash);
    Height = std::max(Height, Child.Height + 1);
    Size += Child.Size;
  }
  ExactHash =
      xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(ExactHashes)});
}

void HashNode::makePostOrder(std::vector<const HashNode *> &PostOrder,
                             const HashNode *Iter) const {
  for (const auto &Child : Iter->Children)
    makePostOrder(PostOrder, &Child);
  PostOrder.push_back(Iter);
}

HashNode::HashNode(const TSNode &RawNode, const SourceCode &Code,
                   bool IsLeaf) noexcept
    : Symbol{ts_node_symbol(RawNode)}, Type(ts_node_type(RawNode)),
      ByteRange{ts_node_start_byte(RawNode), ts_node_end_byte(RawNode)},
      Value(IsLeaf ? Code.getSubstring(ts_node_start_byte(RawNode),
                                       ts_node_end_byte(RawNode))
                   : ""),
      PosRange(UTF8Range{Code.getUTF8Position(ts_node_start_byte(RawNode)),
                         Code.getUTF8Position(ts_node_end_byte(RawNode))}) {}

void HashNode::toStringRecursively(std::string &Buffer, std::size_t Depth,
                                   std::size_t Indent) const {
  Buffer.append(Depth * Indent, ' ').append(toString()).push_back('\n');
  for (auto &Child : Children)
    Child.toStringRecursively(Buffer, Depth + 1, Indent);
}

void HashNode::makeStructuralHashRecursively() {
  if (isLeaf()) {
    StructuralHash = SymbolHash;
    return;
  }

  std::vector<xxh::hash_t<BitMode>> StructuralHashes;
  StructuralHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeStructuralHashRecursively();
    StructuralHashes.push_back(Child.StructuralHash);
  }
  StructuralHash = xxh::xxhash3<BitMode>(
      {SymbolHash, xxh::xxhash3<BitMode>(StructuralHashes)});
}

std::vector<const HashNode *> HashNode::makePostOrder() const {
  std::vector<const HashNode *> PostOrder;
  PostOrder.reserve(Size);
  makePostOrder(PostOrder, this);
  return PostOrder;
}

std::string HashNode::toString() const {
  std::string Buffer;
  Buffer.append(Type);
  if (!Value.empty())
    Buffer.append(": \"").append(Value).push_back('"');
  Buffer.append(" ").append(PosRange.toString());
  return Buffer;
}

std::string HashNode::toStringRecursively(std::size_t Indent) const {
  std::string Buffer;
  toStringRecursively(Buffer, 0, Indent);
  Buffer.pop_back();
  return Buffer;
}

std::unique_ptr<HashNode>
HashNode::build(TSNode RootNode, const SourceCode &Code,
                std::unordered_set<std::string> &Ignore) {
  if (ts_node_is_null(RootNode) || ts_node_is_error(RootNode))
    return nullptr;

  auto Root = std::make_unique<HashNode>(RootNode, Code, false);
  auto Cursor = ts_tree_cursor_new(RootNode);

  if (ts_tree_cursor_goto_first_child(&Cursor))
    do
      if (!build(Code, Cursor, *Root, Ignore))
        return nullptr;
    while (ts_tree_cursor_goto_next_sibling(&Cursor));

  ts_tree_cursor_delete(&Cursor);
  Root->makeMetadataRecursively();
  return Root;
}

} // namespace diffink