#ifndef DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H
#define DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H

#include <cstddef>

namespace diffink {

namespace gumtree {

constexpr std::size_t DefaultMinHeight{1};

constexpr double DefaultMinDice{0.5};

constexpr std::size_t DefaultMaxSize{1000};

} // namespace gumtree

} // namespace diffink

#endif // DIFFINK_TREEDIFF_GUMTREE_HYPERPARAMETERS_H