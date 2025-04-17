#include "DiffInk.h"
#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tree-sitter-cpp.h>
#include <tree-sitter-python.h>
#include <tree_sitter/api.h>

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
           "  int x = 1;\n"
           "  if (x) {\n"
           "    foo();\n"
           "  }\n"
           "}";
  NewStr = "void NewMain() {\n"
           "  int x = x;\n"
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

  // OldStr = "int main() {"
  //          "  int x = 1 + 2 + 3 + 4;\n"
  //          "  int y = 5 + 6 + 7 + 8;\n"
  //          "  int z = 6 + 6 + 6 + 6;}";
  // NewStr = "int main() {"
  //          "  int x = 1 + 2 + 3 + 4;\n"
  //          "  int z = 6 + 6 + 6 + 6;}";

  // OldStr = "int main () { class A { int x; };\n"
  //          "class B { int x; };\n"
  //          "class C { int x; }; }";
  // NewStr = "int main () { class A { int x; };\n"
  //          "class C { int x; }; }";

  // OldStr = "vector<int> x = {1};\n"
  //          "vector<int> y = {1};\n"
  //          "vector<int> z = {1};\n";
  // NewStr = "vector<int> x = {1};\n"
  //          "vector<int> z = {1};\n";

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

  // OldStr = "auto buf = \"이것은 한국어 문장입니다.\";";

  // OldStr = "x=foo(xxx);";
  // NewStr = "x=  x+  foo(x,y);";

  // OldStr = "if x:\n"
  //          "  print(x)\n"
  //          "  print(y)\n"
  //          "print(\"Hello\")\n";
  // NewStr = "if x:\n"
  //          "  print(x)\n"
  //          "print(y)\n"
  //          "print(\"Hell\")\n";

  // OldStr = "Node *const node;";
  // NewStr = "Node *constnode;";

  // OldStr = "x = x + x + x;\n"
  //          "y = 1 + 1 + 1;\n";
  // NewStr = "y = 1 + 1 + 1;\n"
  //          "x = x + x + x;\n";

  OldStr = "axes_list = [a for a in self.figure.get_axes()\n"
           "             if a.patch.contains_point(xy)]\n"
           "\n"
           "    if axes_list:\n"
           "        axes = cbook._topmost_artist(axes_list)\n";
  NewStr = "ICSE_list = [a for a in self.figure.get_axes()\n"
           "             if a.patch.contains_point(xy) and a.get_visible()]\n"
           "\n"
           "    if axes_list:\n"
           "        axes = cbook._topmost_artist(axes_list)\n";

  auto OldCode = std::make_unique<diffink::SourceCode>();
  OldCode->newContent("Old", OldStr);
  auto NewCode = std::make_unique<diffink::SourceCode>();
  NewCode->newContent("New", NewStr);
  auto Diff = diffink::diffText(*OldCode, *NewCode);
  // for (const auto &Edit : Diff) {
  //   std::cout << "Edit: " << Edit.OldStartByte << ", " << Edit.OldEndByte
  //             << ", " << Edit.NewStartByte << ", " << Edit.NewEndByte <<
  //             "\n";
  // }

  diffink::SmartParser Parser(tree_sitter_python);
  diffink::MerkleTree OldTree;
  OldTree.parse(Parser.get(), *OldCode);
  diffink::MerkleTree NewTree;
  NewTree.parse(Parser.get(), OldTree, *OldCode, *NewCode, Diff);

  std::cout << "OldTree:\n";
  std::cout << OldTree.getRoot().toStringRecursively() << "\n";
  std::cout << "NewTree:\n";
  std::cout << NewTree.getRoot().toStringRecursively() << "\n";
}
