#include "DiffInk/Utils/TextDiff.h"

namespace diffink {

bool checkIncparsingAvailable(const SourceCode &OldCode,
                              const SourceCode &NewCode, uint32_t MaxLineDiff) {
  if ((std::max(OldCode.getSize(), NewCode.getSize()) -
       std::min(OldCode.getSize(), NewCode.getSize())) > MaxLineDiff)
    return false;

  auto hashLine = [](const SourceCode &Code, uint32_t StartRow,
                     uint32_t EndRow) -> wchar_t {
    auto Start = Code.getLines()[StartRow];
    auto End = Code.getLines()[EndRow];
    return static_cast<wchar_t>(
        xxhString32(Code.getContent() + Start, End - Start));
  };
  auto hashLastLine = [](const SourceCode &Code, uint32_t StartRow) -> wchar_t {
    auto Start = Code.getLines()[StartRow];
    return static_cast<wchar_t>(
        xxhString32(Code.getContent() + Start, Code.getSize() - Start));
  };

  std::wstring OldLines(OldCode.getLOC(), '\0');
  for (std::size_t i{1}; i != OldCode.getLOC(); ++i)
    OldLines[i - 1] = hashLine(OldCode, i - 1, i);
  OldLines.back() = hashLastLine(OldCode, OldCode.getLOC() - 1);

  std::wstring NewLines(NewCode.getLOC(), '\0');
  for (std::size_t i{1}; i != NewCode.getLOC(); ++i)
    NewLines[i - 1] = hashLine(NewCode, i - 1, i);
  NewLines.back() = hashLastLine(NewCode, NewCode.getLOC() - 1);

  std::size_t DiffCount{0};
  diff_match_patch<std::wstring> Diff;
  Diff.Diff_Timeout = 0.0f;
  for (auto &Edit : Diff.diff_main(OldLines, NewLines, false))
    if (Edit.operation != diff_match_patch<std::wstring>::Operation::EQUAL) {
      DiffCount += Edit.text.length();
      if (DiffCount > MaxLineDiff)
        return false;
    }
  return true;
}

bool applyTextEdits(const SourceCode &OldCode, const SourceCode &NewCode,
                    TSTree &EditedTree, uint32_t MaxLineDiff) {
  if (!checkIncparsingAvailable(OldCode, NewCode, MaxLineDiff))
    return false;

  std::vector<EditedRange> Edits;
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
        Edits.push_back(*CurEdit);
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
    Edits.push_back(*CurEdit);
  }

  for (auto &Edit : Edits) {
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
  return true;
}

} // namespace diffink