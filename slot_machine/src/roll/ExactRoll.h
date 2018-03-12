#ifndef SLOT_MACHINE_EXACTROLL_H
#define SLOT_MACHINE_EXACTROLL_H

#include <vector>
#include "Roll.h"

struct ExactRoll: Roll {
    ExactRoll(const std::vector<double> &speeds, int nframes);
    
    double get_speed(int i) override;
    
    bool is_exhausted() const override;
    
    void step() override;
    
    virtual ~ExactRoll() = default;

protected:
    ExactRoll(int ncells, int nframes);
    
    void set_speed(int i, double new_speed);
private:
    std::vector<double> speeds;
    int nframes;
};


#endif //SLOT_MACHINE_EXACTROLL_H
