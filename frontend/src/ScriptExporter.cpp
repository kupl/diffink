#include "ScriptExporter.h"

std::string ScriptExporter::exportAsString() const {
  std::string Buffer;
  for (const auto &Action : Script) {
    std::visit(
        [&Buffer](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          constexpr std::string_view BigSeperator = "===\n";
          constexpr std::string_view SmallSeperator = "---\n";

          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.append(BigSeperator)
                .append("insert-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.append(BigSeperator)
                .append("delete-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString());

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.append(BigSeperator)
                .append("move-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.append(BigSeperator)
                .append("update-node\n")
                .append(SmallSeperator)
                .append(Action.Leaf.toString())
                .append("\nreplace \"")
                .append(Action.Leaf.getLabel())
                .append("\" by \"")
                .append(Action.UpdatedLeaf.getLabel())
                .push_back('"');

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>)
            Buffer.append(BigSeperator)
                .append("insert-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively())
                .append("\nto\n")
                .append(Action.Parent.toString())
                .append("\nat ")
                .append(std::to_string(Action.Index + 1));

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>)
            Buffer.append(BigSeperator)
                .append("delete-tree\n")
                .append(SmallSeperator)
                .append(Action.Subtree.toStringRecursively());

          Buffer.push_back('\n');
        },
        Action);
  }

  if (!Buffer.empty())
    Buffer.pop_back();
  return Buffer;
}

std::pair<std::string, std::string>
ScriptExporter::exportAsHTML(const diffink::SourceCode &OldSrc,
                             const diffink::SourceCode &NewSrc) const {
  constexpr std::string_view BeginTemplate{R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <style>
    body { background-color: rgb(255, 255, 255);}
  </style>
</head>
<body>
  <pre><code>)"};

  constexpr std::string_view EndTemplate{R"(</code></pre>
</body>
</html>
)"};

  constexpr std::string_view InsertAction{
      R"(<span style="background-color:rgb(159, 255, 159);">)"};
  constexpr std::string_view DeleteAction{
      R"(<span style="background-color:rgb(255, 159, 159);">)"};
  constexpr std::string_view UpdateAction{
      R"(<span style="background-color:rgb(255, 255, 127);">)"};
  constexpr std::string_view MoveAction{
      R"(<span style="background-color:rgb(255, 127, 255);">)"};
  constexpr std::string_view ActionEnd{"</span>"};

  constexpr std::string_view AmpChar{"&amp;"};
  constexpr std::string_view LessChar{"&lt;"};
  constexpr std::string_view GreaterChar{"&gt;"};
  constexpr std::string_view DoubleQuoteChar{"&quot;"};
  constexpr std::string_view SingleQuoteChar{"&apos;"};

  std::vector<std::vector<const std::string_view *>> OldOpenTags(
      OldSrc.getSize() + 1, std::vector<const std::string_view *>()),
      OldCloseTags(OldSrc.getSize() + 1,
                   std::vector<const std::string_view *>()),
      NewOpenTags(NewSrc.getSize() + 1,
                  std::vector<const std::string_view *>()),
      NewCloseTags(NewSrc.getSize() + 1,
                   std::vector<const std::string_view *>());

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>) {
            NewOpenTags[Action.Leaf.getByteRange().first].push_back(
                &InsertAction);
            NewCloseTags[Action.Leaf.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>) {
            OldOpenTags[Action.Leaf.getByteRange().first].push_back(
                &DeleteAction);
            OldCloseTags[Action.Leaf.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>) {
            OldOpenTags[Action.Leaf.getByteRange().first].push_back(
                &UpdateAction);
            OldCloseTags[Action.Leaf.getByteRange().second].push_back(
                &ActionEnd);
            NewOpenTags[Action.UpdatedLeaf.getByteRange().first].push_back(
                &UpdateAction);
            NewCloseTags[Action.UpdatedLeaf.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::MoveTree>) {
            OldOpenTags[Action.Subtree.getByteRange().first].push_back(
                &MoveAction);
            OldCloseTags[Action.Subtree.getByteRange().second].push_back(
                &ActionEnd);
            NewOpenTags[Action.MovedSubtree.getByteRange().first].push_back(
                &MoveAction);
            NewCloseTags[Action.MovedSubtree.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>) {
            NewOpenTags[Action.Subtree.getByteRange().first].push_back(
                &InsertAction);
            NewCloseTags[Action.Subtree.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>) {
            OldOpenTags[Action.Subtree.getByteRange().first].push_back(
                &DeleteAction);
            OldCloseTags[Action.Subtree.getByteRange().second].push_back(
                &ActionEnd);
          }
        },
        Action);
  }

  auto showLineNumber = [](uint32_t &LineNum,
                           uint32_t DigitCount) -> std::string {
    auto Line = std::to_string(LineNum++);
    return R"(<span style="background-color:rgb(223, 223, 223);">)" +
           std::string(DigitCount - Line.size(), ' ') + Line +
           R"( |</span><span style="background-color:rgb(255, 255, 255);"> </span>)";
  };

  const uint32_t OldLineEnd{OldSrc.getLOC() + 1};
  const uint32_t OldDigitCount = std::to_string(OldLineEnd).size();
  uint32_t OldLineNum{1};
  std::string OldBuf(BeginTemplate);
  OldBuf.append(showLineNumber(OldLineNum, OldDigitCount));
  auto OldCStr = OldSrc.getContent();

  for (std::size_t i{0}; i != OldSrc.getSize(); ++i) {
    for (auto Tag : OldCloseTags[i])
      OldBuf.append(*Tag);
    for (auto Tag : OldOpenTags[i])
      OldBuf.append(*Tag);

    switch (OldCStr[i]) {
    case '\n':
      OldBuf.push_back(OldCStr[i]);
      OldBuf.append(showLineNumber(OldLineNum, OldDigitCount));
      break;
    case '&':
      OldBuf.append(AmpChar);
      break;
    case '<':
      OldBuf.append(LessChar);
      break;
    case '>':
      OldBuf.append(GreaterChar);
      break;
    case '"':
      OldBuf.append(DoubleQuoteChar);
      break;
    case '\'':
      OldBuf.append(SingleQuoteChar);
      break;
    default:
      OldBuf.push_back(OldCStr[i]);
    }
  }
  for (auto Tag : OldCloseTags[OldSrc.getSize()])
    OldBuf.append(*Tag);

  const uint32_t NewLineEnd{NewSrc.getLOC() + 1};
  const uint32_t NewDigitCount = std::to_string(NewLineEnd).size();
  uint32_t NewLineNum{1};
  std::string NewBuf(BeginTemplate);
  NewBuf.append(showLineNumber(NewLineNum, NewDigitCount));
  auto NewCStr = NewSrc.getContent();

  for (std::size_t i{0}; i != NewSrc.getSize(); ++i) {
    for (auto Tag : NewOpenTags[i])
      NewBuf.append(*Tag);
    for (auto Tag : NewCloseTags[i])
      NewBuf.append(*Tag);

    switch (NewCStr[i]) {
    case '\n':
      NewBuf.push_back(NewCStr[i]);
      NewBuf.append(showLineNumber(NewLineNum, NewDigitCount));
      break;
    case '&':
      NewBuf.append(AmpChar);
      break;
    case '<':
      NewBuf.append(LessChar);
      break;
    case '>':
      NewBuf.append(GreaterChar);
      break;
    case '"':
      NewBuf.append(DoubleQuoteChar);
      break;
    case '\'':
      NewBuf.append(SingleQuoteChar);
      break;
    default:
      NewBuf.push_back(NewCStr[i]);
    }
  }
  for (auto Tag : NewCloseTags[NewSrc.getSize()])
    NewBuf.append(*Tag);

  OldBuf.append(EndTemplate);
  NewBuf.append(EndTemplate);
  return {OldBuf, NewBuf};
}

nlohmann::ordered_json ScriptExporter::exportAsJSON() const {
  using json = nlohmann::ordered_json;
  auto Buffer = json::array();
  for (const auto &Action : Script) {
    std::visit(
        [&Buffer](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Buffer.push_back(json{{"action", "insert-node"},
                                  {"insert", Action.Leaf.toString()},
                                  {"to", Action.Parent.toString()},
                                  {"at", Action.Index}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Buffer.push_back(json{{"action", "delete-node"},
                                  {"delete", Action.Leaf.toString()}});

          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Buffer.push_back(json{{"action", "move-tree"},
                                  {"move", Action.Subtree.toString()},
                                  {"to", Action.Parent.toString()},
                                  {"at", Action.Index},
                                  {"moved", Action.MovedSubtree.toString()}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Buffer.push_back(json{{"action", "update-node"},
                                  {"replace", Action.Leaf.toString()},
                                  {"by", Action.UpdatedLeaf.toString()}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>)
            Buffer.push_back(json{{"action", "insert-tree"},
                                  {"insert", Action.Subtree.toString()},
                                  {"to", Action.Parent.toString()},
                                  {"at", Action.Index}});

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>)
            Buffer.push_back(json{{"action", "delete-tree"},
                                  {"delete", Action.Subtree.toString()}});
        },
        Action);
  }
  return Buffer;
}