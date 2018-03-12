#ifndef SLOT_MACHINE_NEXTSTEPROLL_H
#define SLOT_MACHINE_NEXTSTEPROLL_H

#include <vector>
#include "ExactRoll.h"

struct NextStepRoll final: ExactRoll {
    NextStepRoll(int nframes, const std::vector<int> &value_counts);
};

#endif //SLOT_MACHINE_NEXTSTEPROLL_H
