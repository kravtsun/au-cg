#ifndef SLOT_MACHINE_ROLL_H
#define SLOT_MACHINE_ROLL_H

struct Roll {
    virtual double get_speed(int i) = 0;
    
    virtual bool is_exhausted() const = 0;
    
    virtual void step() = 0;
    
    virtual ~Roll() = default;
};

#endif //SLOT_MACHINE_ROLL_H
