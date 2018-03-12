#ifndef SLOT_MACHINE_RANDOMROLL_H
#define SLOT_MACHINE_RANDOMROLL_H

#include "ExactRoll.h"

struct RandomRoll final: ExactRoll {
    RandomRoll(int nframes, const std::vector<int> &value_counts, int min_loops=2, int max_loops=5);
};


#endif //SLOT_MACHINE_RANDOMROLL_H
