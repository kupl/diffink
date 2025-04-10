#include "Parser/TextDiff.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tree-sitter-cpp.h>
#include <tree_sitter/api.h>

// 재귀적으로 TSNode를 문자열로 변환하는 헬퍼 함수
std::string nodeToString(TSNode node, int indent = 0) {
  std::ostringstream oss;

  // 들여쓰기를 추가합니다.
  for (int i = 0; i < indent; ++i) {
    oss << "  ";
  }

  // 현재 노드의 타입과 바이트 위치를 출력합니다.
  oss << ts_node_type(node) << " (" << ts_node_start_byte(node) << ", "
      << ts_node_end_byte(node) << ")\n";

  // 자식 노드들을 재귀적으로 처리합니다.
  uint32_t childCount = ts_node_child_count(node);
  for (uint32_t i = 0; i < childCount; ++i) {
    TSNode child = ts_node_child(node, i);
    oss << nodeToString(child, indent + 1);
  }

  return oss.str();
}

// TSTree를 입력받아 전체 트리의 문자열 표현을 반환하는 함수
std::string treeToString(TSTree *tree) {
  TSNode root = ts_tree_root_node(tree);
  return nodeToString(root);
}

int main() {
  std::string OldStr = "int main() {\n"
                       "    int x = 10;\n"
                       "    std::cout << x;"
                       "}";

  std::string NewStr = "int main() {\n"
                       "    int y = 200;\n"
                       "    std::cout << x + 1;"
                       "}";

  OldStr = "void main() {\n"
           "  int x;\n"
           "  if (x) {\n"
           "    foo();\n"
           "  }\n"
           "}";
  NewStr = "void gain() {\n"
           "  int x;\n"
           "  if (x)\n"
           "    foo();\n"
           "}";

  // OldStr = "int main() {\n"
  //          "x = 1 + 2 + 3 + 4;\n"
  //          "}";
  // NewStr = "int main() {\n"
  //          "y = 5 + 6 + 7 + 8;\n"
  //          "if (cond) x = 1 + 2 + 3 + 4;\n"
  //          "}";

  // OldStr = "int main() {\n"
  //          "x = 1 + 2 + 3 + 4;\n"
  //          "y = 5 + 6 + 7 + 8;\n"
  //          "}";
  // NewStr = "int main() {\n"
  //          "y = 5 + 6 + 7 + 8;\n"
  //          "x = 1 + 2 + 3 + 4;\n"
  //          "}";

  OldStr = "  int x = 1 + 2 + 3 + 4;\n"
           "  int y = 5 + 6 + 7 + 8;\n"
           "  int z = 6 + 6 + 6 + 6;";
  NewStr = "  int x = 1 - 2 - 3 - 4;\n"
           "  int z = 6 + 6 + 6 + 6;\n";

  // OldStr = "int main () { class A { int x; };\n"
  //          "class B { int x; };\n"
  //          "class C { int x; }; }";
  // NewStr = "int main () { class A { int x; };\n"
  //          "class C { int x; }; }";

  OldStr = "foo(x);";
  NewStr = "foo();";

  auto Parser = ts_parser_new();
  ts_parser_set_language(Parser, tree_sitter_cpp());

  auto OldTree =
      ts_parser_parse_string(Parser, nullptr, OldStr.c_str(), OldStr.size());

  auto OldTreeCopy = ts_tree_copy(OldTree);
  for (const auto &edit : diffink::diffString(OldStr, NewStr)) {
    std::cout << "---------------------------------\n";
    std::cout << "start_byte: " << edit.start_byte << ", ";
    std::cout << "old_end_byte: " << edit.old_end_byte << ", ";
    std::cout << "new_end_byte: " << edit.new_end_byte << "\n";
    std::cout << "start_point: (" << edit.start_point.row << ", "
              << edit.start_point.column << "), ";
    std::cout << "old_end_point: (" << edit.old_end_point.row << ", "
              << edit.old_end_point.column << "), ";
    std::cout << "new_end_point: (" << edit.new_end_point.row << ", "
              << edit.new_end_point.column << ")\n";
    std::cout << "---------------------------------\n";

    ts_tree_edit(OldTreeCopy, &edit);
  }

  auto NewTree = ts_parser_parse_string(Parser, OldTreeCopy, NewStr.c_str(),
                                        NewStr.size());

  std::cout << "Old Tree:\n";
  std::cout << treeToString(OldTree) << "\n";
  std::cout << "New Tree:\n";
  std::cout << treeToString(NewTree) << "\n";

  uint32_t ChangedRangeSize;
  auto ChangedRangeList =
      ts_tree_get_changed_ranges(OldTreeCopy, NewTree, &ChangedRangeSize);
  std::cout << "Changed Range Size: " << ChangedRangeSize << "\n";
  for (uint32_t i = 0; i != ChangedRangeSize; ++i) {
    auto range = ChangedRangeList[i];
    std::cout << "---------------------------------\n";
    std::cout << "start_byte: " << range.start_byte << ", ";
    std::cout << "end_byte: " << range.end_byte << "\n";
    std::cout << "start_point: (" << range.start_point.row << ", "
              << range.start_point.column << "), ";
    std::cout << "end_point: (" << range.end_point.row << ", "
              << range.end_point.column << ")\n";

    auto Node = ts_node_descendant_for_byte_range(
        ts_tree_root_node(NewTree), range.start_byte, range.end_byte);
    std::cout << ts_node_type(Node) << " (" << ts_node_start_byte(Node) << ", "
              << ts_node_end_byte(Node) << ")\n";
  }
}
