#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <cassert>
#include "RandomRoll.h"

RandomRoll::RandomRoll(int nframes, const std::vector<int> &value_counts, int min_loops, int max_loops)
        : ExactRoll(static_cast<int>(value_counts.size()), nframes)
{
    if (min_loops > max_loops) {
        throw std::logic_error("Invalid arguments to RandomRoll");
    }
    auto const ncells = value_counts.size();
    std::vector<int> choices;
    std::transform(value_counts.cbegin(), value_counts.cend(), std::back_inserter(choices), [](int value_count) {
        assert(value_count > 0);
        return rand() % value_count;
    });
    std::vector<double> speeds;
    assert(choices.size() == ncells);
    std::transform(choices.cbegin(), choices.cend(), value_counts.cbegin(), std::back_inserter(speeds),
                   [&](int choice, int nvalues) {
                       auto const nloops = rand() % (max_loops - min_loops + 1) + min_loops;
                       auto const distance = nloops + choice * (1. / nvalues);
                       return distance / nframes;
                   });
    for (size_t i = 0; i < ncells; ++i) {
        set_speed(i, speeds[i]);
    }
}
