#ifndef DIFFINK_TREEDIFF_GUMTREE_RECOVERY_BASERECOVERY_H
#define DIFFINK_TREEDIFF_GUMTREE_RECOVERY_BASERECOVERY_H

#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::gumtree {

class BaseRecovery {
public:
  virtual void match(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) = 0;
  virtual ~BaseRecovery() = default;
};

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_GUMTREE_RECOVERY_BASERECOVERY_H
