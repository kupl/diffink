#include "Parser/TextDiff.h"

namespace diffink {

std::vector<TSInputEdit> diffText(const SourceCode &OldCode,
                                  const SourceCode &NewCode) {
  return diffText(OldCode, NewCode, getGeneralOpeningSymbols());
}

std::vector<TSInputEdit>
diffText(const SourceCode &OldCode, const SourceCode &NewCode,
         const std::unordered_set<char> &OpeningSymbols) {
  auto OldStr = OldCode.getContent();
  auto NewStr = NewCode.getContent();
  // Dynamic programming table for Myers-Diff algorithm
  std::vector<std::vector<uint32_t>> DistanceTable(
      OldCode.getContentSize(),
      std::vector<uint32_t>(NewCode.getContentSize(), 0));

  // Initialize the distance table and the mapping
  for (uint32_t o = 0; o != OldCode.getContentSize(); ++o)
    DistanceTable[o][0] = o;
  for (uint32_t n = 0; n != NewCode.getContentSize(); ++n)
    DistanceTable[0][n] = n;

  // Fill the distance table
  for (uint32_t o = 1; o != OldCode.getContentSize(); ++o)
    for (uint32_t n = 1; n != NewCode.getContentSize(); ++n)
      DistanceTable[o][n] =
          OldStr[o - 1] == NewStr[n - 1]
              ? DistanceTable[o - 1][n - 1]
              : std::min(DistanceTable[o - 1][n], DistanceTable[o][n - 1]) + 1;

  std::vector<TSInputEdit> InputEditList;
  std::optional<TSInputEdit> CurrentEdit;

  // Helper function to push the current TSInputEdit to the list
  auto pushCurrentEdit = [&]() {
    if (CurrentEdit) {
      CurrentEdit->start_point = OldCode[CurrentEdit->start_byte];
      CurrentEdit->old_end_point = OldCode[CurrentEdit->old_end_byte];
      CurrentEdit->new_end_point = NewCode[CurrentEdit->new_end_byte];
      InputEditList.insert(InputEditList.cbegin(), *CurrentEdit);
      CurrentEdit.reset();
    }
  };

  // Backtrack to find the edits
  for (uint32_t o = OldCode.getCstringSize(), n = NewCode.getCstringSize();
       o != 0 || n != 0;) {
    if (o == 0) {
      if (!CurrentEdit)
        CurrentEdit = TSInputEdit{0, 0, n};
      break;
    }

    if (n == 0) {
      if (!CurrentEdit)
        CurrentEdit = TSInputEdit{0, o, 0};
      else
        CurrentEdit->start_byte = 0;
      break;
    }

    if (OldStr[o - 1] == NewStr[n - 1]) {
      if (OpeningSymbols.contains(OldStr[o - 1])) {
        if (DistanceTable[o][n] == DistanceTable[o][n - 1] + 1) {
          if (!CurrentEdit)
            CurrentEdit = TSInputEdit{o, o, n};
          --n;
          continue;
        }
        if (DistanceTable[o][n] == DistanceTable[o - 1][n] + 1) {
          if (!CurrentEdit)
            CurrentEdit = TSInputEdit{o - 1, o, n};
          else
            CurrentEdit->start_byte = o - 1;
          --o;
          continue;
        }
      }
      pushCurrentEdit();
      --o;
      --n;
      continue;
    }

    if (DistanceTable[o][n] == DistanceTable[o][n - 1] + 1) {
      if (!CurrentEdit)
        CurrentEdit = TSInputEdit{o, o, n};
      --n;
    } else /* DistanceTable[o][n] == DistanceTable[o - 1][n] + 1 */ {
      if (!CurrentEdit)
        CurrentEdit = TSInputEdit{o - 1, o, n};
      else
        CurrentEdit->start_byte = o - 1;
      --o;
    }
  }
  pushCurrentEdit();

  int64_t ByteOffset{0};
  // Update the changed ranges
  for (auto &Edit : InputEditList) {
    Edit.start_byte += ByteOffset;
    Edit.old_end_byte += ByteOffset;
    ByteOffset += static_cast<int64_t>(Edit.new_end_byte) -
                  static_cast<int64_t>(Edit.old_end_byte);

    auto UpdatedStartPoint = NewCode[Edit.start_byte];
    auto RowOffset = static_cast<int64_t>(UpdatedStartPoint.row) -
                     static_cast<int64_t>(Edit.start_point.row);
    auto ColOffset = static_cast<int64_t>(UpdatedStartPoint.column) -
                     static_cast<int64_t>(Edit.start_point.column);

    if (Edit.start_point.row == Edit.old_end_point.row)
      Edit.old_end_point.column += ColOffset;
    Edit.old_end_point.row += RowOffset;
    Edit.start_point = UpdatedStartPoint;
  }

  return InputEditList;
}

std::unordered_set<char> getGeneralOpeningSymbols() noexcept {
  return {'(', '{', '[', '<'};
}

} // namespace diffink
