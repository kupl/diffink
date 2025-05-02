#include "DiffInk/Api.h"
#include "DiffInk/Frontend/ScriptExporter.h"
#include <bitset>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tree-sitter-cpp.h>
#include <tree-sitter-java.h>
#include <tree-sitter-python.h>
#include <tree_sitter/api.h>

std::string read(const std::filesystem::path &FilePath) {
  std::ifstream File(FilePath, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + FilePath.string());

  const auto FileSize = std::filesystem::file_size(FilePath);
  auto Content = std::string(FileSize, '\0');

  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + FilePath.string());
  return Content;
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

  // OldStr = "x = a;\n"
  //          "y = b;\n";
  // NewStr = "y = b;\n"
  //          "x = a;\n";

  // OldStr = "class C {public: void C();};\n"
  //          "class C {private: void D();};\n";
  // NewStr = "class C {private: int D();};\n"
  //          "class C {public: void C();};\n";

  // OldStr = "axes_list = [a for a in self.figure.get_axes()\n"
  //          "             if a.patch.contains_point(xy)]\n"
  //          "\n"
  //          "    if axes_list:\n"
  //          "        axes = cbook._topmost_artist(axes_list)\n";
  // NewStr = "ICSE_list = [a for a in self.figure.get_axes()\n"
  //          "             if a.patch.contains_point(xy) and
  //          a.get_visible()]\n"
  //          "\n"
  //          "    if axes_list:\n"
  //          "        axes = cbook._topmost_artist(axes_list)\n";

  // std::cout << "OldStr:\n" << OldStr << "\n";
  // std::cout << "NewStr:\n" << NewStr << "\n";
  // diffink::SourceCode OldCode(std::move(OldStr));
  // diffink::SourceCode NewCode(std::move(NewStr));

  diffink::SourceCode OldCode(
      read("/home/donguk/diffink_benchmark/d4j/lang_30_buggy/src/main/java/org/"
           "apache/commons/lang3/StringUtils.java"));
  diffink::SourceCode NewCode(
      read("/home/donguk/diffink_benchmark/d4j/lang_30_fixed/src/main/java/org/"
           "apache/commons/lang3/StringUtils.java"));

  auto Diff = diffink::diffText(OldCode, NewCode);

  // for (const auto &Edit : Diff) {
  //   std::cout << OldCode.getPosition(Edit.OldStartByte).row << ","
  //             << OldCode.getPosition(Edit.OldStartByte).column << " "
  //             << OldCode.getPosition(Edit.OldEndByte).row << ","
  //             << OldCode.getPosition(Edit.OldEndByte).column << "\n";
  // }
  // return 0;

  diffink::SmartParser Parser(tree_sitter_java);
  diffink::MerkleTree OldTree, NewTree;
  OldTree.parse(Parser.get(), OldCode);
  NewTree.parseIncrementally(Parser.get(), OldTree, OldCode, NewCode, Diff);
  // NewTree.parse(Parser.get(), NewCode);

  // std::cout << "OldTree:\n";
  // std::cout << OldTree.getRoot().toStringRecursively() << "\n";
  // std::cout << "NewTree:\n";
  // std::cout << NewTree.getRoot().toStringRecursively() << "\n\n\n";

  /**/
  auto Start = std::chrono::steady_clock::now();
  /**/

  auto TestMatcher = diffink::makeGumtreeSimple();
  auto Script = diffink::TreeDiff::runDiff(TestMatcher.get(), OldTree, NewTree);
  auto ExScript = diffink::simplifyEditScript(Script);

  /**/
  auto End = std::chrono::steady_clock::now();
  auto Time = std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
                  .count();
  std::cout << "Running Time: " << Time / 1000 << "." << Time % 1000 << "ms\n";
  /**/

  std::cout << Script.size() << "\n";
  std::cout << ExScript.size() << "\n";

  diffink::frontend::ScriptExporter Exporter(ExScript);
  const auto [OldHTML, NewHTML] = Exporter.exportAsHTML(OldCode, NewCode);
  std::ofstream OldFile("logs/Old.html");
  std::ofstream NewFile("logs/New.html");
  OldFile << OldHTML;
  NewFile << NewHTML;
}
