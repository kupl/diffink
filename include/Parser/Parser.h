#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <memory>
#include <tree_sitter/api.h>

namespace diffink {

class SmartParser {
private:
  std::unique_ptr<const TSLanguage, decltype(&ts_language_delete)> Language;
  std::unique_ptr<TSParser, decltype(&ts_parser_delete)> Parser;

public:
  template <class LangAllocator>
  SmartParser(LangAllocator &&Allocator) noexcept
      : Language(Allocator(), ts_language_delete),
        Parser(ts_parser_new(), ts_parser_delete) {
    ts_parser_set_language(Parser.get(), Language.get());
  }

  TSParser &get() noexcept { return *Parser; }
};

} // namespace diffink

#endif // PARSER_PARSER_H