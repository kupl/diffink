#ifndef TREEDIFF_MATCHER_H
#define TREEDIFF_MATCHER_H

#include "TreeDiff/EditAction.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace diffink {

class Matcher {
public:
  Matcher() noexcept = default;

  // virtual void match(std::unique_ptr<HashNode::Set> OldTree,
  //                    std::unique_ptr<HashNode::Set> NewTree) = 0;

  EditScript makeEditScript() const;
};

} // namespace diffink

#endif // TREEDIFF_MATCHER_H