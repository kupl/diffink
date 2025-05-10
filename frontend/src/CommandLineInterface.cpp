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
      .help("selects matcher algorithm: gumtree-simple, gumtree-opt, "
            "gumtree-opt-inf")
      .choices("gumtree-simple", "gumtree-opt", "gumtree-opt-inf")
      .default_value(std::string{"gumtree-simple"});

  Program.add_argument("-f", "--format")
      .help("selects diff report format: text, html, json")
      .nargs(argparse::nargs_pattern::any)
      .default_value(std::vector<std::string>{"text"});

  Program.add_argument("-o", "--output")
      .help("specifies directory to save diffink outputs")
      .default_value(std::string{"diffink_reports/"});

  Program.add_argument("-r", "--raw")
      .help("disables DiffInk's tree differencing algorithms")
      .implicit_value(true)
      .default_value(false);

  Program.add_argument("--log")
      .help("log file will be saved as \"${OUTPUT}/log\"")
      .implicit_value(true)
      .default_value(false);

  // Program.add_argument("--dump-syntax-tree")
  //     .help("stores syntax tree of input codes; "
  //           "JSON format will be saved in \"${OUTPUT}/\"")
  //     .default_value(false)
  //     .implicit_value(true);
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

#if defined(FRONTEND_PLATFORM_LINUX)
void CommandLineInterface::setDiffinkDirectory() {
  char Buffer[PATH_MAX];
  if (auto Size = readlink("/proc/self/exe", Buffer, PATH_MAX);
      Size != -1 && Size < PATH_MAX)
    DiffinkDirectory = std::filesystem::path(
                           std::string(Buffer, static_cast<std::size_t>(Size)))
                           .parent_path()
                           .parent_path()
                           .parent_path()
                           .parent_path();
}

#elif defined(FRONTEND_PLATFORM_MACOS)
void CommandLineInterface::setDiffinkDirectory() {
  if (uint32_t Size{0}; _NSGetExecutablePath(nullptr, &Size) == -1) {
    std::string<char> buffer(Size);
    if (_NSGetExecutablePath(buffer.data(), &Size) == 0)
      DiffinkDirectory = fs::path(buffer.data())
                             .parent_path()
                             .parent_path()
                             .parent_path()
                             .parent_path();
  }
}
#endif

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
  throw std::invalid_argument("Invalid language: " + Arg);
}

void CommandLineInterface::setMatcher(const std::string &Arg) {
  if (Arg == "gumtree-simple")
    Matcher = diffink::makeGumtreeSimple();
  else if (Arg == "gumtree-opt")
    Matcher = diffink::makeGumtreeOptimal();
  else if (Arg == "gumtree-opt-inf")
    Matcher =
        diffink::makeGumtreeOptimal(std::numeric_limits<std::size_t>::max());
  else
    throw std::invalid_argument("Invalid matcher: " + Arg);
}

void CommandLineInterface::setFormats(const std::vector<std::string> &Args) {
  for (const auto &Arg : Args) {
    if (Arg == "text")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsText(Exporter); });
    else if (Arg == "html")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsHTML(Exporter); });
    else if (Arg == "json")
      Formats.emplace_back(
          [this](const ScriptExporter &Exporter) { exportAsJSON(Exporter); });
    else
      throw std::invalid_argument("Invalid format: " + Arg);
  }
}

void CommandLineInterface::setOutputDirectory(const std::string &Arg) {
  OutputDirectory = Arg;
  if (!std::filesystem::exists(OutputDirectory))
    std::filesystem::create_directories(OutputDirectory);
}

void CommandLineInterface::exportAsText(const ScriptExporter &Exporter) const {
  std::ofstream File(OutputDirectory / "diff.txt");
  if (!File)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff.txt").string());
  File << Exporter.exportAsString();
}

void CommandLineInterface::setLogger() {
  Logger = std::ofstream(OutputDirectory / "log");
  if (!Logger)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "log").string());
}

void CommandLineInterface::exportAsHTML(const ScriptExporter &Exporter) const {
  auto [Old, New] = Exporter.exportAsHTML(*OldCode, *NewCode);
  std::ofstream FileOld(OutputDirectory / "diff_original.html");
  if (!FileOld)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff_original.html").string());
  std::ofstream FileNew(OutputDirectory / "diff_modified.html");
  if (!FileNew)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff_modified.html").string());
  FileOld << Old;
  FileNew << New;
}

void CommandLineInterface::exportAsJSON(const ScriptExporter &Exporter) const {
  std::ofstream File(OutputDirectory / "diff.json");
  if (!File)
    throw std::runtime_error("Failed to open file: " +
                             (OutputDirectory / "diff.json").string());
  File << Exporter.exportAsJSON().dump(2);
}

diffink::ExtendedEditScript CommandLineInterface::runDiffInk() {
  {
    auto Start = std::chrono::steady_clock::now();
    OldTree.parse(Parser->get(), *OldCode);
    auto End = std::chrono::steady_clock::now();

    if (Logger) {
      auto Time =
          std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
              .count();
      *Logger << std::format("Parsing original code took: {}.{:03} ms",
                             Time / 1000, Time % 1000)
              << std::endl;
    }
  }
  {
    auto Start = std::chrono::steady_clock::now();
    NewTree.incparse(Parser->get(), OldTree, *OldCode, *NewCode,
                     diffink::diffText(*OldCode, *NewCode));
    auto End = std::chrono::steady_clock::now();

    if (Logger) {
      auto Time =
          std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
              .count();
      *Logger << std::format("Parsing modified code took: {}.{:03} ms",
                             Time / 1000, Time % 1000)
              << std::endl;
    }
  }

  auto Start = std::chrono::steady_clock::now();
  auto Script = diffink::TreeDiff::runDiffInk(Matcher.get(), OldTree, NewTree);
  auto End = std::chrono::steady_clock::now();

  if (Logger) {
    auto Time =
        std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
            .count();
    *Logger << std::format("Differencing took: {}.{:03} ms", Time / 1000,
                           Time % 1000)
            << std::flush;
  }
  return diffink::simplifyEditScript(Script);
}

diffink::ExtendedEditScript CommandLineInterface::runRawDiff() {
  {
    auto Start = std::chrono::steady_clock::now();
    OldTree.parse(Parser->get(), *OldCode);
    auto End = std::chrono::steady_clock::now();

    if (Logger) {
      auto Time =
          std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
              .count();
      *Logger << std::format("Parsing original code took: {}.{:03} ms",
                             Time / 1000, Time % 1000)
              << std::endl;
    }
  }
  {
    auto Start = std::chrono::steady_clock::now();
    NewTree.parse(Parser->get(), *NewCode);
    auto End = std::chrono::steady_clock::now();

    if (Logger) {
      auto Time =
          std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
              .count();
      *Logger << std::format("Parsing modified code took: {}.{:03} ms",
                             Time / 1000, Time % 1000)
              << std::endl;
    }
  }

  auto Start = std::chrono::steady_clock::now();
  auto Script = diffink::TreeDiff::runDiff(Matcher.get(), OldTree, NewTree);
  auto End = std::chrono::steady_clock::now();

  if (Logger) {
    auto Time =
        std::chrono::duration_cast<std::chrono::microseconds>(End - Start)
            .count();
    *Logger << std::format("Differencing took: {}.{:03} ms", Time / 1000,
                           Time % 1000)
            << std::flush;
  }
  return diffink::simplifyEditScript(Script);
}

void CommandLineInterface::logMeta() {
  if (Logger)
    *Logger << std::format("Method: {}\n"
                           "Matcher: {}\n"
                           "Language: {}\n"
                           "Original: \"{}\"\n"
                           "Modified: \"{}\"\n"
                           "========",
                           IsRaw ? "raw" : "diffink",
                           Program.get<std::string>("--matcher"),
                           Program.get<std::string>("--language"),
                           Program.get<std::string>("original"),
                           Program.get<std::string>("modified"))
            << std::endl;
}

void CommandLineInterface::makeReport(
    diffink::ExtendedEditScript &&Script) const {
  ScriptExporter Exporter(Script);
  for (const auto &Format : Formats)
    Format(Exporter);
}

void CommandLineInterface::run() {
  initArguments();
  setDiffinkDirectory();
  Program.parse_args(argc, argv);
  setParser(Program.get<std::string>("--language"));
  setMatcher(Program.get<std::string>("--matcher"));
  setFormats(Program.get<std::vector<std::string>>("--format"));
  setOutputDirectory(Program.get<std::string>("--output"));
  IsRaw = Program.get<bool>("--raw");

  if (Program.get<bool>("--log"))
    setLogger();
  logMeta();

  OldCode = std::make_unique<diffink::SourceCode>(
      read(Program.get<std::string>("original")));
  NewCode = std::make_unique<diffink::SourceCode>(
      read(Program.get<std::string>("modified")));

  if (IsRaw)
    makeReport(runRawDiff());
  else
    makeReport(runDiffInk());

  std::exit(0);
}