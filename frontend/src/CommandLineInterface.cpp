#include "CommandLineInterface.h"

void CommandLineInterface::initArguments() {
  Program.add_argument("original").help("locates original code file");
  Program.add_argument("modified").help("locates modified code file");

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
    LangHelp.append("none\n");
  else {
    for (const auto &lang : Languages)
      LangHelp.append(lang).append(", ");
    LangHelp.pop_back();
    LangHelp.pop_back();
  }
  {
    auto &Temp =
        Program.add_argument("-l", "--language").required().help(LangHelp);
    for (const auto &lang : Languages)
      Temp.add_choice(lang);
  }

  Program.add_argument("-m", "--matcher")
      .help("selects matcher algorithm: gumtree-simple, gumtree-opt-1000, "
            "gumtree-opt-inf")
      .choices("gumtree-simple", "gumtree-opt-1000", "gumtree-opt-inf")
      .default_value(std::string{"gumtree-simple"});

  Program.add_argument("-f", "--format")
      .help("selects diff report format: text, html, json")
      .choices("text", "html", "json")
      .nargs(argparse::nargs_pattern::any)
      .default_value(std::vector<std::string>{"text"});

  Program.add_argument("-o", "--output")
      .help("specifies directory to save diffink outputs")
      .default_value(std::string{"diffink_reports/"});

  Program.add_argument("-r", "--raw")
      .help("disables DiffInk's tree differencing algorithms")
      .default_value(false)
      .implicit_value(true);

  Program.add_argument("--dump-syntax-tree")
      .help("stores syntax tree of input codes\n"
            "JSON format will be saved in \"${OUTPUT}/\"")
      .default_value(false)
      .implicit_value(true);

  Program.add_argument("--logging")
      .help("enables logging\n"
            "log file will be saved as \"${OUTPUT}/log\"")
      .default_value(false)
      .implicit_value(true);
}

std::string
CommandLineInterface::read(const std::filesystem::path &Path) const {
  std::ifstream File(Path, std::ios::binary);
  if (!File)
    throw std::runtime_error("Failed to open file: " + Path.string());
  const auto FileSize = std::filesystem::file_size(Path);
  std::string Content(FileSize, '\0');
  if (!File.read(Content.data(), FileSize))
    throw std::runtime_error("Failed to read file: " + Path.string());
  return Content;
}

void CommandLineInterface::setParser(const std::string &Arg) {
#ifdef DIFFINK_LANGUAGE_SUPPORT_C
  if (Arg == "c") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_c);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSHARP
  if (Arg == "csharp") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_c_sharp);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CPP
  if (Arg == "cpp") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_cpp);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_CSS
  if (Arg == "css") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_css);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_GO
  if (Arg == "go") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_go);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HASKELL
  if (Arg == "haskell") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_haskell);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_HTML
  if (Arg == "html") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_html);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVA
  if (Arg == "java") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_java);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JAVASCRIPT
  if (Arg == "javascript") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_javascript);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JSON
  if (Arg == "json") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_json);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_JULIA
  if (Arg == "julia") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_julia);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_OCAML
  if (Arg == "ocaml") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_ocaml);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PHP
  if (Arg == "php") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_php);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_PYTHON
  if (Arg == "python") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_python);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUBY
  if (Arg == "ruby") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_ruby);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_RUST
  if (Arg == "rust") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_rust);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_SCALA
  if (Arg == "scala") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_scala);
    return;
  }
#endif
#ifdef DIFFINK_LANGUAGE_SUPPORT_TYPESCRIPT
  if (Arg == "typescript") {
    Parser = std::make_unique<diffink::SmartParser>(tree_sitter_typescript);
    return;
  }
#endif
  throw std::runtime_error("Invalid language: " + Arg);
}

void CommandLineInterface::setMatcher(const std::string &Arg) {
  if (Arg == "gumtree-simple")
    Matcher = diffink::makeGumtreeSimple();
  else if (Arg == "gumtree-opt-1000")
    Matcher = diffink::makeGumtreeOptimal(1000);
  else if (Arg == "gumtree-opt-inf")
    Matcher =
        diffink::makeGumtreeOptimal(std::numeric_limits<std::size_t>::max());
  else
    throw std::runtime_error("Invalid matcher: " + Arg);
}

void CommandLineInterface::setOutputDirectory(const std::string &Arg) {
  // if (Arg.empty())
  //   OutputStream = std::make_unique<std::ostream>(std::cout.rdbuf());
  // else {
  //   auto FileStream = std::make_unique<std::ofstream>(Arg);
  //   if (FileStream && FileStream->is_open())
  //     OutputStream = std::move(FileStream);
  //   throw std::runtime_error("Failed to open output file: " + Arg);
  // }
}

void CommandLineInterface::run() {
  initArguments();
  Program.parse_args(argc, argv);

  setParser(Program.get<std::string>("--language"));
  setMatcher(Program.get<std::string>("--matcher"));

  std::exit(0);
}