#ifndef FRONTEND_COMMANDLINEINTERFACE_H
#define FRONTEND_COMMANDLINEINTERFACE_H

#include "DiffInk/Api.h"
#include <argparse.hpp>
#include <filesystem>
#include <fstream>

#ifdef DIFFINK_LANGUAGE_SUPPORT_C
#include <tree-sitter-c.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
#include <tree-sitter-csharp.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
#include <tree-sitter-cpp.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
#include <tree-sitter-css.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
#include <tree-sitter-go.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
#include <tree-sitter-haskell.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
#include <tree-sitter-html.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
#include <tree-sitter-java.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
#include <tree-sitter-javascript.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
#include <tree-sitter-json.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
#include <tree-sitter-julia.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_OCAML
#include <tree-sitter-ocaml.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PHP
#include <tree-sitter-php.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
#include <tree-sitter-python.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
#include <tree-sitter-ruby.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
#include <tree-sitter-rust.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
#include <tree-sitter-scala.h>
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_TYPESCRIPT
#include <tree-sitter-typescript.h>
#endif

class CommandLineInterface {
private:
  int argc;
  char **argv;
  argparse::ArgumentParser Program;

  std::unique_ptr<diffink::SmartParser> Parser;
  std::unique_ptr<diffink::TreeDiff::Matcher> Matcher;
  std::filesystem::path OutputDiretory;
  bool IsRaw;
  bool DumpSyntaxTree;
  bool Logging;

private:
  void initArguments();

  std::string read(const std::filesystem::path &Path) const;

  void setParser(const std::string &Arg);

  void setMatcher(const std::string &Arg);

  void setOutputDirectory(const std::string &Arg);

  void exportAsText(const diffink::ExtendedEditScript &Script) const;

public:
  CommandLineInterface(int argc, char *argv[]) noexcept
      : argc{argc}, argv{argv}, Program("DiffInk", "1.0") {}

  void run();
};

#endif // FRONTEND_COMMANDLINEINTERFACE_H