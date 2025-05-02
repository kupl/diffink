#ifndef DIFFINK_TREEDIFF_DICESIMILARITY_H
#define DIFFINK_TREEDIFF_DICESIMILARITY_H

#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::gumtree {

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             const std::unordered_set<VirtualNode *> &New);

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             VirtualNode *New);

} // namespace diffink::gumtree

#endif // DIFFINK_TREEDIFF_DICESIMILARITY_H