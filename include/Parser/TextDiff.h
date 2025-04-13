#ifndef PARSER_TEXTDIFF_H
#define PARSER_TEXTDIFF_H

#include "Parser/SourceCode.h"
#include <optional>
#include <unordered_set>

namespace diffink {

std::vector<TSInputEdit> diffText(const SourceCode &OldCode,
                                  const SourceCode &NewCode);

std::vector<TSInputEdit>
diffText(const SourceCode &OldCode, const SourceCode &NewCode,
         const std::unordered_set<char> &OpeningSymbols);

std::unordered_set<char> getGeneralOpeningSymbols() noexcept;

} // namespace diffink

#endif // PARSER_TEXTDIFF_H