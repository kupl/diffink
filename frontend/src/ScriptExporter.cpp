#include "ScriptExporter.h"

std::string ScriptExporter::exportAsString() const {
  std::string Buffer;
  for (const auto &Action : Script) {
    Buffer.append(std::visit(
        [](const auto &Action) { return Action.toString(); }, Action));
  }
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
</head>
<body>
  <pre><code>)"};

  constexpr std::string_view EndTemplate{R"(</code></pre>
</body>
</html>
)"};

  const std::string_view InsertAction{
      R"(<span style="background-color:rgb(159, 255, 159);">)"};
  const std::string_view DeleteAction{
      R"(<span style="background-color:rgb(255, 159, 159);">)"};
  const std::string_view UpdateAction{
      R"(<span style="background-color:rgb(255, 255, 127);">)"};
  const std::string_view MoveAction{
      R"(<span style="background-color:rgb(255, 159, 255);">)"};
  const std::string_view ActionEnd{"</span>"};

  constexpr std::string_view AmpChar{"&amp;"};
  constexpr std::string_view LessChar{"&lt;"};
  constexpr std::string_view GreaterChar{"&gt;"};
  constexpr std::string_view DoubleQuoteChar{"&quot;"};
  constexpr std::string_view SingleQuoteChar{"&apos;"};

  std::vector<std::vector<const std::string_view *>> OldTags(
      OldSrc.getSize() + 1, std::vector<const std::string_view *>());
  std::vector<std::vector<const std::string_view *>> NewTags(
      NewSrc.getSize() + 1, std::vector<const std::string_view *>());

  for (const auto &Action : Script) {
    std::visit(
        [&](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>) {
            NewTags[Action.Leaf.getByteRange().first].push_back(&InsertAction);
            NewTags[Action.Leaf.getByteRange().second].push_back(&ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>) {
            OldTags[Action.Leaf.getByteRange().first].push_back(&DeleteAction);
            OldTags[Action.Leaf.getByteRange().second].push_back(&ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>) {
            OldTags[Action.Leaf.getByteRange().first].push_back(&UpdateAction);
            OldTags[Action.Leaf.getByteRange().second].push_back(&ActionEnd);
            NewTags[Action.UpdatedLeaf.getByteRange().first].push_back(
                &UpdateAction);
            NewTags[Action.UpdatedLeaf.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::MoveTree>) {
            OldTags[Action.Subtree.getByteRange().first].push_back(&MoveAction);
            OldTags[Action.Subtree.getByteRange().second].push_back(&ActionEnd);
            NewTags[Action.MovedSubtree.getByteRange().first].push_back(
                &MoveAction);
            NewTags[Action.MovedSubtree.getByteRange().second].push_back(
                &ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>) {
            NewTags[Action.Subtree.getByteRange().first].push_back(
                &InsertAction);
            NewTags[Action.Subtree.getByteRange().second].push_back(&ActionEnd);
          }

          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>) {
            OldTags[Action.Subtree.getByteRange().first].push_back(
                &DeleteAction);
            OldTags[Action.Subtree.getByteRange().second].push_back(&ActionEnd);
          }
        },
        Action);
  }

  std::string OldBuf(BeginTemplate);
  std::string NewBuf(BeginTemplate);
  auto OldCStr = OldSrc.getContent();
  auto NewCStr = NewSrc.getContent();

  for (std::size_t i{0}; i != OldSrc.getSize(); ++i) {
    for (auto Tag : OldTags[i])
      OldBuf.append(*Tag);

    switch (OldCStr[i]) {
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
  for (auto Tag : OldTags[OldSrc.getSize()])
    OldBuf.append(*Tag);

  for (std::size_t i{0}; i != NewSrc.getSize(); ++i) {
    for (auto Tag : NewTags[i])
      NewBuf.append(*Tag);

    switch (NewCStr[i]) {
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
  for (auto Tag : NewTags[NewSrc.getSize()])
    NewBuf.append(*Tag);

  OldBuf.append(EndTemplate);
  NewBuf.append(EndTemplate);
  return {OldBuf, NewBuf};
}

nlohmann::json
ScriptExporter::exportAsJSON(const diffink::TreeDiff &Mapping) const {
  using json = nlohmann::json;
  auto Buffer =
      json({{"matches", json::array()}, {"edit_script", json::array()}});

  auto &Matches = Buffer["matches"];
  for (const auto [OldNode, NewNode] : Mapping.getMapping())
    Matches.push_back({{"original", OldNode->Original.toString()},
                       {"modified", NewNode->Original.toString()}});

  auto &Actions = Buffer["edit_script"];
  for (const auto &Action : Script) {
    std::visit(
        [&Actions](const auto &Action) {
          using T = std::decay_t<decltype(Action)>;
          if constexpr (std::is_same_v<T, diffink::edit_action::InsertNode>)
            Actions.push_back({{"action", "insert-node"},
                               {"insert", Action.Leaf.toString()},
                               {"to", Action.Parent.toString()},
                               {"at", Action.Index}});
          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteNode>)
            Actions.push_back({{"action", "delete-node"},
                               {"delete", Action.Leaf.toString()}});
          else if constexpr (std::is_same_v<T, diffink::edit_action::MoveTree>)
            Actions.push_back({{"action", "move-tree"},
                               {"move", Action.Subtree.toString()},
                               {"to", Action.Parent.toString()},
                               {"at", Action.Index},
                               {"moved", Action.MovedSubtree.toString()}});
          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::UpdateNode>)
            Actions.push_back({{"action", "update-node"},
                               {"replace", Action.Leaf.toString()},
                               {"by", Action.UpdatedLeaf.toString()}});
          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::InsertTree>)
            Actions.push_back({{"action", "insert-tree"},
                               {"insert", Action.Subtree.toString()},
                               {"to", Action.Parent.toString()},
                               {"at", Action.Index}});
          else if constexpr (std::is_same_v<T,
                                            diffink::edit_action::DeleteTree>)
            Actions.push_back({{"action", "delete-tree"},
                               {"delete", Action.Subtree.toString()}});
        },
        Action);
  }
  return Buffer;
}