#ifndef SLOT_MACHINE_CELLSPASS_H
#define SLOT_MACHINE_CELLSPASS_H

#include "PassthroughPass.h"
#include "CellHolder.h"
#include "color/ColorSupplier.h"

struct CellsPass : PassthroughPass {
    CellsPass(int width, int height);
    
    void pass() override;
    
    void set_fgcolor(const ColorSupplierWrapper &new_fgcolor);
    
    void set_bgcolor(const ColorSupplierWrapper &new_bgcolor);
    
    ~CellsPass() final = default;
    
private:
    std::vector<CellHolder> cell_holders;
    ProgramWrapper program;
    GLint pass_texture_id, ymin_id, ymax_id, bgcolor_id, fgcolor_id;
    ColorSupplierWrapper fgcolor;
    ColorSupplierWrapper bgcolor;
};


#endif //SLOT_MACHINE_CELLSPASS_H
