#include "AST/RawTree.h"
#include "Parser/Index.h"
#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tree-sitter-cpp.h>
#include <tree-sitter-python.h>
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
           "  int z = 6 + 6 + 6 + 6;";

  // OldStr = "int main () { class A { int x; };\n"
  //          "class B { int x; };\n"
  //          "class C { int x; }; }";
  // NewStr = "int main () { class A { int x; };\n"
  //          "class C { int x; }; }";

  // OldStr = "vector<int> x = {1, 2, 3};\n"
  //          "vector<int> z = {1, 2, 3};\n";
  // NewStr = "vector<int> x = {1, 2, 3};\n"
  //          "vector<int> y = {1, 2, 3};\n"
  //          "vector<int> z = {1, 2, 3};\n";

  // OldStr = "if x:\n"
  //          "    print(x)\n"
  //          "else:\n"
  //          "    print(y)\n"
  //          "    print(z)\n";
  // NewStr = "if x:\n"
  //          "    print(x)\n"
  //          "else:\n"
  //          "    print(y)\n"
  //          "print(z)\n";

  // OldStr = "x = [1, 2, 3, 4]\n"
  //          "z = [1, 2, 3, 4]\n";
  // NewStr = "x = [1, 2, 3, 4]\n"
  //          "y = [1, 2, 3, 4]\n"
  //          "z = [1, 2, 3, 4]\n";

  NewStr = "이것은 한국어 문장입니다";

  auto OldCode = std::make_unique<diffink::SourceCode>();
  OldCode->newContent("Old", OldStr);
  auto NewCode = std::make_unique<diffink::SourceCode>();
  NewCode->newContent("New", NewStr);
  for (auto i = 0; i < NewCode->getContentSize(); ++i) {
    // print char as 0bXXXXXXXX format
    std::cout << std::bitset<8>(
                     static_cast<unsigned char>(NewCode->getContent()[i]))
              << " " << (*NewCode)[i].row << " " << (*NewCode)[i].column
              << "\n";
  }
}
