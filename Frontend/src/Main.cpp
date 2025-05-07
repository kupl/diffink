#include "CommandLineInterface.h"
#include "ScriptExporter.h"
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

int main(int argc, char *argv[]) {
  CommandLineInterface Cli(argc, argv);
  Cli.run();

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

  std::cout << "OldStr:\n" << OldStr << "\n";
  std::cout << "NewStr:\n" << NewStr << "\n";
  diffink::SourceCode OldCode(std::move(OldStr));
  diffink::SourceCode NewCode(std::move(NewStr));

  // diffink::SourceCode OldCode(
  //     read("/home/donguk/diffink_benchmark/d4j/lang_30_buggy/src/main/java/org/"
  //          "apache/commons/lang3/StringUtils.java"));
  // diffink::SourceCode NewCode(
  //     read("/home/donguk/diffink_benchmark/d4j/lang_30_fixed/src/main/java/org/"
  //          "apache/commons/lang3/StringUtils.java"));

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

  auto TestMatcher = diffink::makeGumtreeOptimal();
  auto Script =
      diffink::TreeDiff::runDiffInk(TestMatcher.get(), OldTree, NewTree);
  auto ExScript = diffink::simplifyEditScript(Script);

  /**/
  auto End = std::chrono::steady_clock::now();
  auto Time = std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
                  .count();
  std::cout << "Running Time: " << Time / 1000 << "." << Time % 1000 << "ms\n";
  /**/

  std::cout << Script.size() << "\n";
  std::cout << ExScript.size() << "\n";

  ScriptExporter Exporter(ExScript);
  const auto [OldHTML, NewHTML] = Exporter.exportAsHTML(OldCode, NewCode);
  std::ofstream OldFile("logs/old.html");
  std::ofstream NewFile("logs/new.html");
  OldFile << OldHTML;
  NewFile << NewHTML;
}