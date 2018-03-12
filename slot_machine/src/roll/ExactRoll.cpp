#include "ExactRoll.h"

double ExactRoll::get_speed(int i) {
    return speeds.at(i);
}

bool ExactRoll::is_exhausted() const {
    return nframes <= 0;
}

bool ExactRoll::step() {
    if (nframes > 0) {
        nframes--;
    }
}

void ExactRoll::set_speed(int i, double new_speed) {
    speeds.at(i) = new_speed;
}

ExactRoll::ExactRoll(int ncells, int nframes)
        : ExactRoll(std::vector<double>(ncells, 0), nframes)
{}

ExactRoll::ExactRoll(const std::vector<double> &speeds, int nframes)
        : speeds(speeds)
        , nframes(nframes)
{}
