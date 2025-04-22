#include "AST/MerkleTree.h"

namespace diffink {

void MerkleTree::identifyChange(MerkleTree &OldTree, Iterator Iters) {
  bool HasEditedChild = ts_tree_cursor_goto_first_child(&Iters.OldCursor);
  bool HasNewChild = ts_tree_cursor_goto_first_child(&Iters.NewCursor);

  auto insertStructuralChange =
      [](MerkleTree &Tree, const HashNode &Iter,
         std::list<HashNode>::const_iterator Child) -> void {
    Tree.ChangedNodes.insert(&*Child);
    Tree.HasChangedChild.insert(&Iter);
  };

  if (HasEditedChild && HasNewChild) {
    for (auto OldHashChild = Iters.OldHashIter.getChildren().cbegin(),
              NewHashChild = Iters.NewHashIter.getChildren().cbegin(),
              OldHashEnd = Iters.OldHashIter.getChildren().cend(),
              NewHashEnd = Iters.NewHashIter.getChildren().cend();
         ;) {

      if (OldHashChild == OldHashEnd) {
        for (; NewHashChild != NewHashEnd; ++NewHashChild)
          ChangedNodes.insert(&*NewHashChild);
        HasChangedChild.insert(&Iters.NewHashIter);
        break;
      }

      if (NewHashChild == NewHashEnd) {
        for (; OldHashChild != OldHashEnd; ++OldHashChild)
          OldTree.ChangedNodes.insert(&*OldHashChild);
        OldTree.HasChangedChild.insert(&Iters.OldHashIter);
        break;
      }

      switch (compareNodes(ts_tree_cursor_current_node(&Iters.OldCursor),
                           ts_tree_cursor_current_node(&Iters.NewCursor))) {
      case NodeComp::Equal:
        if (OldHashChild->getSymbol() == NewHashChild->getSymbol()) {
          Mapping.emplace_back(&*OldHashChild, &*NewHashChild);

          if (!HashNode::isExactlyEqual(*OldHashChild, *NewHashChild)) {
            identifyChange(OldTree, {*OldHashChild, Iters.OldCursor,
                                     *NewHashChild, Iters.NewCursor});
            OldTree.HasChangedChild.insert(&Iters.OldHashIter);
            HasChangedChild.insert(&Iters.NewHashIter);
          }
        }

        else {
          insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
          insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        }

        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Inequal:
        if (!HashNode::isExactlyEqual(*OldHashChild, *NewHashChild)) {
          insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
          insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        }

        else
          Mapping.emplace_back(&*OldHashChild, &*NewHashChild);

        ++OldHashChild;
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        break;

      case NodeComp::Precedes:
        insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
        ++OldHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        break;

      case NodeComp::Succeeds:
        insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        ++NewHashChild;
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
      }
    }

    ts_tree_cursor_goto_parent(&Iters.OldCursor);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
  }

  else if (!HasEditedChild && !HasNewChild) {
    if (!HashNode::isExactlyEqual(Iters.OldHashIter, Iters.NewHashIter))
      Mapping.emplace_back(&Iters.OldHashIter, &Iters.NewHashIter);
  }

  else if (HasEditedChild) {
    for (auto &Child : Iters.OldHashIter.getChildren())
      OldTree.ChangedNodes.insert(&Child);
    OldTree.HasChangedChild.insert(&Iters.OldHashIter);
    ts_tree_cursor_goto_parent(&Iters.OldCursor);
  }

  else /* HasNewChild */ {
    for (auto &Child : Iters.NewHashIter.getChildren())
      ChangedNodes.insert(&Child);
    HasChangedChild.insert(&Iters.NewHashIter);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
  }
}

void MerkleTree::clearChanges() noexcept {
  Mapping.clear();
  decltype(Mapping)().swap(Mapping);
  ChangedNodes.clear();
  decltype(ChangedNodes)().swap(ChangedNodes);
  HasChangedChild.clear();
  decltype(HasChangedChild)().swap(HasChangedChild);
}

void MerkleTree::swap(MerkleTree &Rhs) noexcept {
  Root.swap(Rhs.Root);
  RawTree.swap(Rhs.RawTree);
  Mapping.swap(Rhs.Mapping);
  ChangedNodes.swap(Rhs.ChangedNodes);
  HasChangedChild.swap(Rhs.HasChangedChild);
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

bool MerkleTree::parseIncrementally(TSParser &Parser, MerkleTree &OldTree,
                                    const SourceCode &OldCode,
                                    const SourceCode &Code,
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

  auto OldCursor = ts_tree_cursor_new(ts_tree_root_node(EditedTree));
  auto NewCursor = ts_tree_cursor_new(ts_tree_root_node(RawTree.get()));
  identifyChange(OldTree, {OldTree.getRoot(), OldCursor, *Root, NewCursor});

  ts_tree_cursor_delete(&OldCursor);
  ts_tree_cursor_delete(&NewCursor);
  ts_tree_delete(EditedTree);
  return true;
}

} // namespace diffink