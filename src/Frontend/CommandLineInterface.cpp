#include "DiffInk/Frontend/CommandLineInterface.h"

namespace diffink::frontend {

void CommandLineInterface::setArguments() {
  ArgParser.add_argument("original code").help("locates original code file");

  ArgParser.add_argument("modified code").help("locates modified code file");

  std::vector<std::string> Languages;
#ifdef DIFFINK_LANGUAGE_SUPPORT_C
  Languages.emplace_back("c");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
  Languages.emplace_back("csharp");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
  Languages.emplace_back("cpp");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
  Languages.emplace_back("css");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
  Languages.emplace_back("go");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
  Languages.emplace_back("haskell");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
  Languages.emplace_back("html");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
  Languages.emplace_back("java");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
  Languages.emplace_back("javascript");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
  Languages.emplace_back("json");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
  Languages.emplace_back("julia");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_OCAML
  Languages.emplace_back("ocaml");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PHP
  Languages.emplace_back("php");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
  Languages.emplace_back("python");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
  Languages.emplace_back("ruby");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
  Languages.emplace_back("rust");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
  Languages.emplace_back("scala");
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_TYPESCRIPT
  Languages.emplace_back("typescript");
#endif

  std::string LangHelp{"specifies language of input codes\n"
                       "supported languages: "};
  if (Languages.empty())
    LangHelp.append("none");
  else {
    for (const auto &lang : Languages)
      LangHelp.append(lang).append(", ");
    LangHelp.pop_back();
    LangHelp.pop_back();
  }

  Languages.emplace_back("auto");
  ArgParser.add_argument("-l", "--language")
      .required()
      .help(LangHelp)
      .choices(Languages);

  ArgParser.add_argument("-f", "--format")
      .help("selects difference representation format:\n"
            "text(default), html, json")
      .default_value(std::string{"text"})
      .choices("text", "html", "json");

  ArgParser.add_argument("-o", "--output")
      .help("specifies difference output path\n"
            "if not specified, output will be printed to stdout")
      .default_value(std::string{"stdout"});

  ArgParser.add_argument("-r", "--raw")
      .help("disables DiffInk's tree differencing algorithms")
      .default_value(false)
      .implicit_value(true);

  ArgParser.add_argument("--dump-syntax-tree")
      .help("stores syntax tree of input codes\n"
            "JSON format will be saved in {diffink_dir}/logs")
      .default_value(false)
      .implicit_value(true);

  ArgParser.add_argument("--logging")
      .help("enables logging\n"
            "log file will be saved in {diffink_dir}/logs")
      .default_value(false)
      .implicit_value(true);
}

CommandLineInterface::CommandLineInterface(int argc, char *argv[])
    : ArgParser("DiffInk") {
  setArguments();
  ArgParser.parse_args(argc, argv);
}

} // namespace diffink::frontend