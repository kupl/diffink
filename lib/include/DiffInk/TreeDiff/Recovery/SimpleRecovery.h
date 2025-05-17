#ifndef DIFFINK_TREEDIFF_RECOVERY_SIMPLERECOVERY_H
#define DIFFINK_TREEDIFF_RECOVERY_SIMPLERECOVERY_H

#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::recovery {

class SimpleRecovery : public TreeDiff::Recovery {
private:
  template <class Comparator>
  void matchLcs(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New,
                Comparator &&Equal) const;

  void matchUnique(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New);

public:
  SimpleRecovery() noexcept = default;

  ~SimpleRecovery() = default;

  void match(TreeDiff &Mapping, VirtualNode *Old, VirtualNode *New) final;
};

} // namespace diffink::recovery

#endif // DIFFINK_TREEDIFF_RECOVERY_SIMPLERECOVERY_H