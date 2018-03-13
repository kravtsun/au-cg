#ifndef SLOT_MACHINE_RESOURCES_H
#define SLOT_MACHINE_RESOURCES_H

#include <string>

#define RESOURCES_PATH "resources/"
#define START_BUTTON_TEXT_PATH RESOURCES_PATH "start1.bmp"
#define FONT_PATH RESOURCES_PATH "arial.ttf"

static inline std::string SYMBOL_PATH(char c) {
    return RESOURCES_PATH + std::string(1, c) + ".bmp";
}

#endif //SLOT_MACHINE_RESOURCES_H
