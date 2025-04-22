#include "AST/HashNode.h"

namespace diffink {

std::string HashNode::UTF8Range::toString() const {
  return "(" + std::to_string(StartPos.row) + "," +
         std::to_string(StartPos.column) + ")-(" + std::to_string(EndPos.row) +
         "," + std::to_string(EndPos.column) + ")";
}

void HashNode::makeMetadataRecursively() {
  SymbolHash = xxh::xxhash3<BitMode>(&Symbol, sizeof(Symbol));
  Size = 1;

  if (Children.empty()) {
    ExactHash =
        xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(Value)});
    Height = 1;
    return;
  }

  std::vector<xxh::hash_t<BitMode>> ExactHashes;
  ExactHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeMetadataRecursively();
    ExactHashes.push_back(Child.ExactHash);

    Size += Child.Size;
    Height = std::max(Height, Child.Height + 1);
  }
  ExactHash =
      xxh::xxhash3<BitMode>({SymbolHash, xxh::xxhash3<BitMode>(ExactHashes)});
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

void HashNode::toStringRecursively(std::string &Buffer,
                                   std::size_t CurrentIndentSize,
                                   std::size_t Indent) const {
  Buffer.append(CurrentIndentSize, ' ').append(toString()).push_back('\n');
  for (auto &Child : Children)
    Child.toStringRecursively(Buffer, CurrentIndentSize + Indent, Indent);
}

void HashNode::makeStructuralHashRecursively() {
  if (isLeaf()) {
    StructuralHash = SymbolHash;
    Height = 1;
    return;
  }

  std::vector<xxh::hash_t<BitMode>> StructuralHashes;
  StructuralHashes.reserve(Children.size());

  for (auto &Child : Children) {
    Child.makeStructuralHashRecursively();
    StructuralHashes.push_back(Child.StructuralHash);
    if (Child.Height >= Height)
      Height = Child.Height + 1;
  }
  StructuralHash = xxh::xxhash3<BitMode>(
      {SymbolHash, xxh::xxhash3<BitMode>(StructuralHashes)});
}

bool HashNode::build(const SourceCode &Code, TSTreeCursor &Cursor,
                     HashNode &Parent) {
  auto Node = ts_tree_cursor_current_node(&Cursor);
  if (ts_node_is_error(Node))
    return false;

  if (!ts_tree_cursor_goto_first_child(&Cursor))
    Parent.Children.emplace_back(Node, Code, true);
  else {
    auto &HashNode = Parent.Children.emplace_back(Node, Code, false);
    do
      if (!build(Code, Cursor, HashNode))
        return false;
    while (ts_tree_cursor_goto_next_sibling(&Cursor));
    ts_tree_cursor_goto_parent(&Cursor);
  }
  return true;
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

std::unique_ptr<HashNode> HashNode::build(TSNode RootNode,
                                          const SourceCode &Code) {
  if (ts_node_is_null(RootNode) || ts_node_is_error(RootNode))
    return nullptr;

  auto Root = std::make_unique<HashNode>(RootNode, Code, false);
  auto Cursor = ts_tree_cursor_new(RootNode);

  if (ts_tree_cursor_goto_first_child(&Cursor))
    do
      if (!build(Code, Cursor, *Root))
        return nullptr;
    while (ts_tree_cursor_goto_next_sibling(&Cursor));

  ts_tree_cursor_delete(&Cursor);
  Root->makeMetadataRecursively();
  return Root;
}

} // namespace diffink