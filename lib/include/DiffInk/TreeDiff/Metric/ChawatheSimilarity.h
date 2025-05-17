#ifndef DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H
#define DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H

#include "DiffInk/TreeDiff/TreeDiff.h"
#include <cmath>

namespace diffink::metric {

double computeChawatheSimilarity(TreeDiff &Mapping,
                                 const std::unordered_set<VirtualNode *> &Old,
                                 VirtualNode *New);

double computeAutoChawatheThreshold(VirtualNode *Old,
                                    VirtualNode *New) noexcept;

} // namespace diffink::metric

#endif // DIFFINK_TREEDIFF_METRIC_CHAWATHESIMILARITY_H