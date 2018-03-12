#include "NextStepRoll.h"

NextStepRoll::NextStepRoll(int nframes, const std::vector<int> &value_counts)
        : ExactRoll(static_cast<int>(value_counts.size()), nframes)
{
    for (size_t i = 0; i < value_counts.size(); ++i) {
        set_speed(i, 1. / (value_counts[i] * nframes));
    }
}
