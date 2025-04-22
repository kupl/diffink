#include "Utils/TextDiff.h"

namespace diffink {

EditSequence diffText(const SourceCode &OldCode, const SourceCode &NewCode,
                      const std::set<char> &ClosingSymbols) {
  auto OldStr = OldCode.getContent();
  auto NewStr = NewCode.getContent();
  // Dynamic programming table for Myers-Diff algorithm
  std::vector<std::vector<uint32_t>> DistanceTable(
      OldCode.getContentSize(),
      std::vector<uint32_t>(NewCode.getContentSize(), 0));

  // Initialize the distance table and the mapping
  for (uint32_t o{1}; o != OldCode.getContentSize(); ++o)
    DistanceTable[o][0] = o;
  for (uint32_t n{1}; n != NewCode.getContentSize(); ++n)
    DistanceTable[0][n] = n;

  // Fill the distance table
  for (uint32_t o{1}; o != OldCode.getContentSize(); ++o)
    for (uint32_t n{1}; n != NewCode.getContentSize(); ++n)
      DistanceTable[o][n] =
          OldStr[o - 1] == NewStr[n - 1]
              ? DistanceTable[o - 1][n - 1]
              : std::min(DistanceTable[o - 1][n], DistanceTable[o][n - 1]) + 1;

  EditSequence Seq;
  std::optional<EditedRange> CurrentEdit;

  for (uint32_t o = OldCode.getCstringSize(), n = NewCode.getCstringSize();
       o != 0 || n != 0;) {

    if (o == 0) {
      if (CurrentEdit)
        CurrentEdit->NewStartByte = 0;
      else
        CurrentEdit = EditedRange{0, 0, 0, n};
      break;
    }

    if (n == 0) {
      if (CurrentEdit)
        CurrentEdit->OldStartByte = 0;
      else
        CurrentEdit = EditedRange{0, o, 0, 0};
      break;
    }

    bool IsCommonCharacter = OldStr[o - 1] == NewStr[n - 1];
    bool IsInsertAvaliable = DistanceTable[o][n] == DistanceTable[o][n - 1] + 1;
    bool IsDeleteAvaliable = DistanceTable[o][n] == DistanceTable[o - 1][n] + 1;

    if (IsCommonCharacter && (!IsInsertAvaliable && !IsDeleteAvaliable ||
                              ClosingSymbols.contains(OldStr[o - 1]))) {
      if (CurrentEdit) {
        Seq.insert(Seq.cbegin(), *CurrentEdit);
        CurrentEdit.reset();
      }
      --o;
      --n;
    } else if (IsInsertAvaliable) {
      if (CurrentEdit)
        CurrentEdit->NewStartByte = n - 1;
      else
        CurrentEdit = EditedRange{o, o, n - 1, n};
      --n;
    } else /* IsDeleteAvaliable */ {
      if (CurrentEdit)
        CurrentEdit->OldStartByte = o - 1;
      else
        CurrentEdit = EditedRange{o - 1, o, n, n};
      --o;
    }
  }

  if (CurrentEdit) {
    Seq.insert(Seq.cbegin(), *CurrentEdit);
    CurrentEdit.reset();
  }
  return Seq;
}

void applyEditSequence(const SourceCode &OldCode, const SourceCode &NewCode,
                       TSTree &EditedTree, const EditSequence &Seq) {
  for (auto &Edit : Seq) {
    auto TempEdit =
        TSInputEdit{.start_byte = Edit.NewStartByte,
                    .old_end_byte = Edit.NewStartByte +
                                    (Edit.OldEndByte - Edit.OldStartByte),
                    .new_end_byte = Edit.NewEndByte,
                    .start_point = NewCode[Edit.NewStartByte],
                    .old_end_point = OldCode[Edit.OldEndByte],
                    .new_end_point = NewCode[Edit.NewStartByte]};

    if (TempEdit.start_point.row == OldCode[Edit.OldStartByte].row)
      TempEdit.old_end_point.column +=
          static_cast<int64_t>(TempEdit.start_point.column) -
          static_cast<int64_t>(OldCode[Edit.OldStartByte].column);
    TempEdit.old_end_point.row +=
        static_cast<int64_t>(TempEdit.start_point.row) -
        static_cast<int64_t>(OldCode[Edit.OldStartByte].row);
    ts_tree_edit(&EditedTree, &TempEdit);
  }
}

} // namespace diffink