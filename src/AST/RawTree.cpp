#include "AST/RawTree.h"
#include <iostream>
namespace diffink {

void RawTree::swap(RawTree &Rhs) noexcept {
  Tree.swap(Rhs.Tree);
  Code.swap(Rhs.Code);
  ChangedRanges.swap(Rhs.ChangedRanges);
}

void RawTree::parse(TSParser &Parser,
                    std::unique_ptr<SourceCode> Code) noexcept {
  this->Code = std::move(Code);
  Tree.release();
  decltype(ChangedRanges)().swap(ChangedRanges);
  if (this->Code)
    Tree.reset(ts_parser_parse_string(&Parser, nullptr,
                                      this->Code->getContent(),
                                      this->Code->getCstringSize()));
}

void RawTree::parse(TSParser &Parser, std::unique_ptr<SourceCode> Code,
                    const RawTree &OldTree,
                    const std::vector<TSInputEdit> &EditSequence) {
  this->Code = std::move(Code);
  Tree.release();
  decltype(ChangedRanges)().swap(ChangedRanges);
  if (!this->Code)
    return;

  auto OldTreeCopy = ts_tree_copy(OldTree.Tree.get());
  for (const auto &Edit : EditSequence)
    ts_tree_edit(OldTreeCopy, &Edit);
  Tree.reset(ts_parser_parse_string(&Parser, OldTreeCopy,
                                    this->Code->getContent(),
                                    this->Code->getCstringSize()));

  uint32_t TSRangeListSize;
  auto TSRangeList =
      ts_tree_get_changed_ranges(OldTreeCopy, Tree.get(), &TSRangeListSize);
  ChangedRanges.reserve(TSRangeListSize);
  for (uint32_t i = 0; i != TSRangeListSize; ++i)
    ChangedRanges.push_back(TSRangeList[i]);

  std::free(TSRangeList);
  ts_tree_delete(OldTreeCopy);
}

RawNode RawTree::getNodeInfo(TSNode RawNode) const noexcept {
  return {
      .Symbol{ts_node_symbol(RawNode)},
      .Type = ts_node_type(RawNode),
      .Text = Code->getSubstring(ts_node_start_byte(RawNode),
                                 ts_node_end_byte(RawNode)),
      .StartByte{ts_node_start_byte(RawNode)},
      .EndByte{ts_node_end_byte(RawNode)},
  };
}

// std::string RawTree::toString(int Indent) const {
//   if (!Tree)
//     return "";
//   auto Root = ts_tree_root_node(Tree.get());
//   if (ts_node_is_null(Root))
//     return "";

//   auto Cursor = ts_tree_cursor_new(Root);
//   std::string Buffer;
//   int CurrentIndent{0};

//   auto Code = this->Code.get();
//   auto appendPosition = [Code, &Buffer](uint32_t ByteOffset) -> std::string &
//   {
//     Buffer.push_back('[');
//     Buffer.append(std::to_string((*Code)[ByteOffset].row)).append(", ");
//     Buffer.append(std::to_string((*Code)[ByteOffset].column)).push_back(']');
//     return Buffer;
//   };

//   do {
//     auto Node = ts_tree_cursor_current_node(&Cursor);
//     Buffer.append(CurrentIndent, ' ').append(ts_node_type(Node)).push_back('
//     ');

//     auto StartByte = ts_node_start_byte(Node);
//     auto EndByte = ts_node_end_byte(Node);
//     bool IsInternalNode = ts_tree_cursor_goto_first_child(&Cursor);
//     if (!IsInternalNode)
//       Buffer.append(": \"")
//           .append(Code->getSubstring(StartByte, EndByte))
//           .append("\" ");

//     appendPosition(StartByte).append(" - ");
//     appendPosition(EndByte).push_back('\n');

//     if (IsInternalNode) {
//       CurrentIndent += Indent;
//       continue;
//     }

//     if (!ts_tree_cursor_goto_next_sibling(&Cursor))
//       do {
//         CurrentIndent -= Indent;
//         ts_tree_cursor_goto_parent(&Cursor);
//         if (ts_tree_cursor_goto_next_sibling(&Cursor))
//           break;
//       } while (CurrentIndent > 0);
//     if (CurrentIndent <= 0)
//       break;
//   } while (true);

//   ts_tree_cursor_delete(&Cursor);
//   return Buffer;
// }
} // namespace diffink