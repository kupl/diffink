#ifndef DIFFINK_UTILS_TEXTDIFF_H
#define DIFFINK_UTILS_TEXTDIFF_H

#include "DiffInk/AST/SourceCode.h"
#include <diff_match_patch.h>
#include <optional>

namespace diffink {

struct EditedRange {
  uint32_t OldStartByte;
  uint32_t OldEndByte;
  uint32_t NewStartByte;
  uint32_t NewEndByte;
};

using EditSequence = std::vector<EditedRange>;

EditSequence diffText(const SourceCode &OldCode, const SourceCode &NewCode);

void applyEditSequence(const SourceCode &OldCode, const SourceCode &NewCode,
                       TSTree &EditedTree, const EditSequence &Seq);

} // namespace diffink

#endif // DIFFINK_UTILS_TEXTDIFF_H