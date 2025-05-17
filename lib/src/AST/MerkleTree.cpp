#include "DiffInk/AST/MerkleTree.h"

namespace diffink {

void MerkleTree::identifyChange(MerkleTree &OldTree, Iterator Iters) {
  bool HasEditedChild = ts_tree_cursor_goto_first_child(&Iters.OldCursor);
  bool HasNewChild = ts_tree_cursor_goto_first_child(&Iters.NewCursor);

  auto insertStructuralChange =
      [](MerkleTree &Tree, const HashNode &Iter,
         std::list<HashNode>::const_iterator Child) -> void {
    Tree.UncommonNodes.insert(&*Child);
    Tree.HasUncommonChild.insert(&Iter);
  };

  if (HasEditedChild && HasNewChild) {
    for (auto OldHashChild = Iters.OldHashIter.getChildren().cbegin(),
              NewHashChild = Iters.NewHashIter.getChildren().cbegin(),
              OldHashEnd = Iters.OldHashIter.getChildren().cend(),
              NewHashEnd = Iters.NewHashIter.getChildren().cend();
         ;) {

      if (OldHashChild == OldHashEnd) {
        for (; NewHashChild != NewHashEnd; ++NewHashChild)
          UncommonNodes.insert(&*NewHashChild);
        OldTree.HasUncommonChild.insert(&Iters.OldHashIter);
        HasUncommonChild.insert(&Iters.NewHashIter);
        break;
      }

      if (NewHashChild == NewHashEnd) {
        for (; OldHashChild != OldHashEnd; ++OldHashChild)
          OldTree.UncommonNodes.insert(&*OldHashChild);
        OldTree.HasUncommonChild.insert(&Iters.OldHashIter);
        HasUncommonChild.insert(&Iters.NewHashIter);
        break;
      }

      auto OldRawNode = ts_tree_cursor_current_node(&Iters.OldCursor);
      while (ts_node_is_missing(OldRawNode)) {
        ts_tree_cursor_goto_next_sibling(&Iters.OldCursor);
        OldRawNode = ts_tree_cursor_current_node(&Iters.OldCursor);
      }
      auto NewRawNode = ts_tree_cursor_current_node(&Iters.NewCursor);
      while (ts_node_is_missing(NewRawNode)) {
        ts_tree_cursor_goto_next_sibling(&Iters.NewCursor);
        NewRawNode = ts_tree_cursor_current_node(&Iters.NewCursor);
      }

      switch (compareNodes(OldRawNode, NewRawNode)) {
      case NodeComp::Equal:
        if (OldHashChild->getType() == NewHashChild->getType()) {
          Mapping.emplace_back(&*OldHashChild, &*NewHashChild);

          if (!HashNode::eqaulExactly(*OldHashChild, *NewHashChild)) {
            identifyChange(OldTree, {*OldHashChild, Iters.OldCursor,
                                     *NewHashChild, Iters.NewCursor});
            OldTree.HasUncommonChild.insert(&Iters.OldHashIter);
            HasUncommonChild.insert(&Iters.NewHashIter);
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
        if (!HashNode::eqaulExactly(*OldHashChild, *NewHashChild)) {
          insertStructuralChange(OldTree, Iters.OldHashIter, OldHashChild);
          insertStructuralChange(*this, Iters.NewHashIter, NewHashChild);
        } else
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
    if (!HashNode::eqaulExactly(Iters.OldHashIter, Iters.NewHashIter))
      Mapping.emplace_back(&Iters.OldHashIter, &Iters.NewHashIter);
  }

  else if (HasEditedChild) {
    for (auto &Child : Iters.OldHashIter.getChildren())
      OldTree.UncommonNodes.insert(&Child);
    OldTree.HasUncommonChild.insert(&Iters.OldHashIter);
    ts_tree_cursor_goto_parent(&Iters.OldCursor);
  }

  else /* HasNewChild */ {
    for (auto &Child : Iters.NewHashIter.getChildren())
      UncommonNodes.insert(&Child);
    HasUncommonChild.insert(&Iters.NewHashIter);
    ts_tree_cursor_goto_parent(&Iters.NewCursor);
  }
}

void MerkleTree::clearMapping() noexcept {
  Mapping.clear();
  decltype(Mapping)().swap(Mapping);
  UncommonNodes.clear();
  decltype(UncommonNodes)().swap(UncommonNodes);
  HasUncommonChild.clear();
  decltype(HasUncommonChild)().swap(HasUncommonChild);
}

void MerkleTree::swap(MerkleTree &Rhs) noexcept {
  Root.swap(Rhs.Root);
  RawTree.swap(Rhs.RawTree);
  Mapping.swap(Rhs.Mapping);
  UncommonNodes.swap(Rhs.UncommonNodes);
  HasUncommonChild.swap(Rhs.HasUncommonChild);
}

void MerkleTree::clear() noexcept {
  Root.reset();
  RawTree.reset();
  clearMapping();
}

void MerkleTree::parse(TSParser &Parser, const SourceCode &Code) {
  clear();
  RawTree.reset(ts_parser_parse_string(&Parser, nullptr, Code.getContent(),
                                       Code.getSize()));
  Root = HashNode::build(ts_tree_root_node(RawTree.get()), Code);
  Root->makeStructuralHashRecursively();
}

void MerkleTree::incparse(TSParser &Parser, MerkleTree &OldTree,
                          const SourceCode &OldCode, const SourceCode &Code) {
  clear();
  OldTree.clearMapping();
  auto EditedTree = ts_tree_copy(OldTree.RawTree.get());
  if (applyTextEdits(OldCode, Code, *EditedTree)) {
    Incparsed = true;
    RawTree.reset(ts_parser_parse_string(&Parser, EditedTree, Code.getContent(),
                                         Code.getSize()));
  } else
    RawTree.reset(ts_parser_parse_string(&Parser, nullptr, Code.getContent(),
                                         Code.getSize()));

  Root = HashNode::build(ts_tree_root_node(RawTree.get()), Code);
  Root->makeStructuralHashRecursively();
  auto OldCursor = ts_tree_cursor_new(ts_tree_root_node(EditedTree));
  auto NewCursor = ts_tree_cursor_new(ts_tree_root_node(RawTree.get()));
  identifyChange(OldTree, {OldTree.getRoot(), OldCursor, *Root, NewCursor});

  ts_tree_cursor_delete(&OldCursor);
  ts_tree_cursor_delete(&NewCursor);
  ts_tree_delete(EditedTree);
}

} // namespace diffink