#ifndef DIFFINK_UTILS_TEXTDIFF_H
#define DIFFINK_UTILS_TEXTDIFF_H

#include "DiffInk/AST/SourceCode.h"
#include <optional>
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

#endif // DIFFINK_UTILS_TEXTDIFF_H