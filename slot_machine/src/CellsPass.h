#ifndef SLOT_MACHINE_CELLSPASS_H
#define SLOT_MACHINE_CELLSPASS_H

#include <memory>

#include "PassthroughPass.h"
#include "CellHolder.h"
#include "color/ColorSupplier.h"
#include "roll/Roll.h"

struct CellsPass : PassthroughPass {
    CellsPass(int width, int height);
    
    void pass() override;
    
    void set_fgcolor(const ColorSupplierWrapper &new_fgcolor);
    
    void set_bgcolor(const ColorSupplierWrapper &new_bgcolor);
    
    void start();
    
    ~CellsPass() final = default;
    
private:
    std::vector<std::vector<std::string>> cells_lines;
    std::shared_ptr<Roll> roll;
    std::vector<CellHolder> cell_holders;
    ProgramWrapper program;
    GLint pass_texture_id, ymin_id, ymax_id, bgcolor_id, fgcolor_id;
    ColorSupplierWrapper fgcolor, bgcolor;
};


#endif //SLOT_MACHINE_CELLSPASS_H
