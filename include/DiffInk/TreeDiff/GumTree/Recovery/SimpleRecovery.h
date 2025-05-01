#ifndef DIFFINK_TREEDIFF_GUMTREE_SIMPLERECOVERY_H
#define DIFFINK_TREEDIFF_GUMTREE_SIMPLERECOVERY_H

#include "DiffInk/TreeDiff/GumTree/GreedyBottomUp.h"

namespace diffink {

namespace gumtree {

class SimpleRecovery : public GreedyBottomUp::Recovery {
private:
  template <class Comparator>
  void matchLcs(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New,
                Comparator &&Comp) const;

  void matchUnique(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New);

public:
  SimpleRecovery() noexcept = default;

  void match(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) final;
};

} // namespace gumtree

} // namespace diffink

#endif // DIFFINK_TREEDIFF_GUMTREE_SIMPLERECOVERY_H