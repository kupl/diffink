#ifndef PARSER_TEXTDIFF_H
#define PARSER_TEXTDIFF_H

#include <optional>
#include <string>
#include <tree_sitter/api.h>
#include <unordered_set>
#include <vector>

namespace diffink {

std::vector<TSInputEdit> diffString(const std::string &OldStr,
                                    const std::string &NewStr);

std::vector<TSInputEdit>
diffString(const std::string &OldStr, const std::string &NewStr,
           const std::unordered_set<char> &OpeningSymbols);

std::unordered_set<char> getCStyleOpeningSymbols();

std::unordered_set<char> getPythonStyleOpeningSymbols();

} // namespace diffink

#endif // PARSER_TEXTDIFF_H