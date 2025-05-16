#ifndef DIFFINK_UTILS_TEXTDIFF_H
#define DIFFINK_UTILS_TEXTDIFF_H

#include "DiffInk/AST/SourceCode.h"
#include "DiffInk/Utils/XxhStl.h"
#include <cstring>
#include <diff_match_patch.h>
#include <optional>

namespace diffink {

struct EditedRange {
  uint32_t OldStartByte;
  uint32_t OldEndByte;
  uint32_t NewStartByte;
  uint32_t NewEndByte;
};

bool checkIncparsingAvailable(const SourceCode &OldCode,
                              const SourceCode &NewCode, uint32_t MaxLineDiff);

bool applyTextEdits(const SourceCode &OldCode, const SourceCode &NewCode,
                    TSTree &EditedTree, uint32_t MaxLineDiff = 2000);

} // namespace diffink

#endif // DIFFINK_UTILS_TEXTDIFF_H