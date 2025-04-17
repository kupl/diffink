#ifndef UTILS_TEXTDIFF_H
#define UTILS_TEXTDIFF_H

#include "AST/SourceCode.h"
#include <set>

namespace diffink {

struct EditedRange {
  uint32_t OldStartByte;
  uint32_t OldEndByte;
  uint32_t NewStartByte;
  uint32_t NewEndByte;
};

using EditSequence = std::vector<EditedRange>;

constexpr std::set<char> makeGeneralClosingSymbols() {
  return {')', ']', '>', '}'};
}

EditSequence diffText(const SourceCode &OldCode, const SourceCode &NewCode,
                      const std::set<char> &ClosingSymbols);

constexpr EditSequence diffText(const SourceCode &OldCode,
                                const SourceCode &NewCode) {
  return diffText(OldCode, NewCode, makeGeneralClosingSymbols());
}

void applyEditSequence(const SourceCode &OldCode, const SourceCode &NewCode,
                       TSTree &EditedTree, const EditSequence &Seq);

} // namespace diffink

#endif // UTILS_TEXTDIFF_H