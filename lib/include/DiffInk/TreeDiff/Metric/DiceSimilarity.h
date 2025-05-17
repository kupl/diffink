#ifndef DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H
#define DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H

#include "DiffInk/TreeDiff/TreeDiff.h"

namespace diffink::metric {

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             const std::unordered_set<VirtualNode *> &New);

double computeDiceSimilarity(TreeDiff &Mapping,
                             const std::unordered_set<VirtualNode *> &Old,
                             VirtualNode *New);

} // namespace diffink::metric

#endif // DIFFINK_TREEDIFF_METRIC_DICESIMILARITY_H