#include "AST/MerkleTree.h"
#include <iostream>

namespace diffink {

void MerkleTree::identifyChange(MerkleTree &OldTree, Iterators Iters) {
  bool HasEditedChild = ts_tree_cursor_goto_first_child(&Iters.EditedCursor);
  bool HasNewChild = ts_tree_cursor_goto_first_child(&Iters.NewCursor);

  if (HasEditedChild && HasNewChild) {
    for (auto OldHashChild = Iters.OldHashIter.getChildren().cbegin(),
              NewHashChild = Iters.NewHashIter.getChildren().cbegin(),
              OldHashEnd = Iters.OldHashIter.getChildren().cend(),
              NewHashEnd = Iters.NewHashIter.getChildren().cend();
         ;) {
      switch (compareNodes(ts_tree_cursor_current_node(&Iters.EditedCursor),
                           ts_tree_cursor_current_node(&Iters.NewCursor))) {
      case NodeComp::Equal:
        if (OldHashChild->getSymbol() != NewHashChild->getSymbol()) {
          OldTree.StructuralChanges.insert(&*OldHashChild);
          StructuralChanges.insert(&*NewHashChild);

        } else if (!HashNode::isExactlyEqual(*OldHashChild, *NewHashChild))
          identifyChange(OldTree, {*OldHashChild, Iters.EditedCursor,
                                   *NewHashChild, Iters.NewCursor});

        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.EditedCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Precedes:
        OldTree.StructuralChanges.insert(&*OldHashChild);
        ++OldHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.EditedCursor);
        break;

      case NodeComp::Succeeds:
        StructuralChanges.insert(&*NewHashChild);
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Inequal:
        OldTree.StructuralChanges.insert(&*OldHashChild);
        StructuralChanges.insert(&*NewHashChild);
        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.EditedCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
      }

      if (OldHashChild == OldHashEnd) {
        for (; NewHashChild != NewHashEnd; ++NewHashChild)
          StructuralChanges.insert(&*NewHashChild);
        break;
      }

      if (NewHashChild == NewHashEnd) {
        for (; OldHashChild != OldHashEnd; ++OldHashChild)
          OldTree.StructuralChanges.insert(&*OldHashChild);
        break;
      }
    }

    ts_tree_cursor_goto_parent(&Iters.EditedCursor);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
    return;
  }

  if (!HasEditedChild && !HasNewChild) {
    if (!HashNode::isExactlyEqual(Iters.OldHashIter, Iters.NewHashIter)) {
      OldTree.TextualChanges.insert(&Iters.OldHashIter);
      TextualChanges.insert(&Iters.NewHashIter);
    }
    return;
  }

  if (HasEditedChild)
    ts_tree_cursor_goto_parent(&Iters.EditedCursor);
  else if (HasNewChild)
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
  OldTree.StructuralChanges.insert(&Iters.OldHashIter);
  StructuralChanges.insert(&Iters.NewHashIter);
}

void MerkleTree::clearChanges() noexcept {
  StructuralChanges.clear();
  decltype(StructuralChanges)().swap(StructuralChanges);
  TextualChanges.clear();
  decltype(TextualChanges)().swap(TextualChanges);
}

void MerkleTree::swap(MerkleTree &Rhs) noexcept {
  Root.swap(Rhs.Root);
  RawTree.swap(Rhs.RawTree);
  StructuralChanges.swap(Rhs.StructuralChanges);
  TextualChanges.swap(Rhs.TextualChanges);
}

void MerkleTree::clear() noexcept {
  Root.reset();
  RawTree.reset();
  clearChanges();
}

bool MerkleTree::parse(TSParser &Parser, const SourceCode &Code) {
  clear();
  RawTree.reset(ts_parser_parse_string(&Parser, nullptr, Code.getContent(),
                                       Code.getCstringSize()));

  if (Root = HashNode::build(ts_tree_root_node(RawTree.get()), Code))
    return true;
  clear();
  return false;
}

bool MerkleTree::parse(TSParser &Parser, MerkleTree &OldTree,
                       const SourceCode &OldCode, const SourceCode &Code,
                       const EditSequence &Seq) {
  clear();
  OldTree.clearChanges();

  auto EditedTree = ts_tree_copy(OldTree.RawTree.get());
  applyEditSequence(OldCode, Code, *EditedTree, Seq);
  RawTree.reset(ts_parser_parse_string(&Parser, EditedTree, Code.getContent(),
                                       Code.getCstringSize()));
  if (!(Root = HashNode::build(ts_tree_root_node(RawTree.get()), Code))) {
    ts_tree_delete(EditedTree);
    clear();
    return false;
  }

  auto EditedCursor = ts_tree_cursor_new(ts_tree_root_node(EditedTree));
  auto NewCursor = ts_tree_cursor_new(ts_tree_root_node(RawTree.get()));
  identifyChange(OldTree, {OldTree.getRoot(), EditedCursor, *Root, NewCursor});

  std::cout << std::endl << "Old Tree(Structural): " << std::endl;
  for (auto &Change : OldTree.StructuralChanges)
    std::cout << Change->toString() << std::endl;

  std::cout << std::endl << "New Tree(Structural): " << std::endl;
  for (auto &Change : StructuralChanges)
    std::cout << Change->toString() << std::endl;

  std::cout << std::endl << "Old Tree(Textual): " << std::endl;
  for (auto &Change : OldTree.TextualChanges)
    std::cout << Change->toString() << std::endl;

  std::cout << std::endl << "New Tree(Textual): " << std::endl;
  for (auto &Change : TextualChanges)
    std::cout << Change->toString() << std::endl;
  std::cout << std::endl;

  ts_tree_cursor_delete(&EditedCursor);
  ts_tree_cursor_delete(&NewCursor);
  ts_tree_delete(EditedTree);
  return true;
}

EditScript MerkleTree::copyChangedSubtree(ShallowMap &Map,
                                          const MerkleTree &OldTree,
                                          const MerkleTree &NewTree) {
  EditScript Script;
  std::queue<std::pair<const HashNode *, ShallowTree::Node *>> OldQueue;
  std::queue<std::pair<const HashNode *, ShallowTree::Node *>> NewQueue;
  OldQueue.emplace(OldTree.getRoot(), Map.OldTree.getRoot());
  NewQueue.emplace(NewTree.getRoot(), Map.NewTree.getRoot());

  while (!OldQueue.empty() && !NewQueue.empty()) {
    auto [OldHashParent, OldShallowParent] = OldQueue.front();
    auto [NewHashParent, NewShallowParent] = NewQueue.front();

    for (auto OldHashIter = OldHashParent->getChildren().cbegin(),
              NewHashIter = NewHashParent->getChildren().cbegin(),
              OldHashEnd = OldHashParent->getChildren().cend(),
              NewHashEnd = NewHashParent->getChildren().cend();
         ;) {
      bool IsOldChanged = OldTree.StructuralChanges.contains(&*OldHashIter);
      bool IsNewChanged = NewTree.StructuralChanges.contains(&*NewHashIter);
      bool IsOldTextChanged = OldTree.TextualChanges.contains(&*OldHashIter);
      bool IsNewTextChanged = NewTree.TextualChanges.contains(&*NewHashIter);

      if (IsOldChanged || IsNewChanged) {
        if (IsOldChanged) {
          Map.OldTree.buildSubtree(
              *OldHashIter,
              Map.OldTree.pushBack(*OldHashIter, OldShallowParent));
          ++OldHashIter;
        }

        if (IsNewChanged) {
          Map.NewTree.buildSubtree(
              *NewHashIter,
              Map.NewTree.pushBack(*NewHashIter, NewShallowParent));
          ++NewHashIter;
        }
        continue;
      }

      auto InsertedOldNode =
          Map.OldTree.pushBack(*OldHashIter, OldShallowParent);
      auto InsertedNewNode =
          Map.NewTree.pushBack(*NewHashIter, NewShallowParent);
      InsertedOldNode->IsVirtual = true;
      InsertedNewNode->IsVirtual = true;

      if (IsOldTextChanged && IsNewTextChanged)
        Script.push_back(edit_action::UpdateNode{&*OldHashIter, &*NewHashIter});

      else if (!OldHashIter->isLeaf() && !NewHashIter->isLeaf()) {

      } else
        throw std::logic_error("Unreachable code in copyChangedSubtree");
    }
  }
}

} // namespace diffink