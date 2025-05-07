#include "DiffInk/Utils/TextDiff.h"

namespace diffink {

EditSequence diffText(const SourceCode &OldCode, const SourceCode &NewCode) {
  EditSequence Seq;
  diff_match_patch<std::string> Diff;
  Diff.Diff_Timeout = 0.0f;

  uint32_t OldIter{0}, NewIter{0};
  std::optional<EditedRange> CurEdit;

  for (const auto &Edit :
       Diff.diff_main(OldCode.getContent(), NewCode.getContent(), false)) {
    if (Edit.operation == diff_match_patch<std::string>::Operation::EQUAL) {
      if (CurEdit) {
        CurEdit->OldEndByte = OldIter;
        CurEdit->NewEndByte = NewIter;
        Seq.push_back(*CurEdit);
        CurEdit.reset();
      }
      OldIter += Edit.text.length();
      NewIter += Edit.text.length();
      continue;
    }

    if (!CurEdit)
      CurEdit = EditedRange{.OldStartByte = OldIter,
                            .OldEndByte = OldIter,
                            .NewStartByte = NewIter,
                            .NewEndByte = NewIter};

    if (Edit.operation == diff_match_patch<std::string>::Operation::DELETE)
      CurEdit->OldEndByte = OldIter += Edit.text.length();
    else
      CurEdit->NewEndByte = NewIter += Edit.text.length();
  }

  if (CurEdit) {
    CurEdit->OldEndByte = OldIter;
    CurEdit->NewEndByte = NewIter;
    Seq.push_back(*CurEdit);
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