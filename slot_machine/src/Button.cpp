#include <stdexcept>
#include <GL/glew.h>
#include "Button.h"

Button::Button(const glm::ivec2 &position, const glm::ivec2 &size)
        : position(position)
        , size(size)
{
    if (position.x < 0 || position.y < 0 || size.x < 0 || size.y < 0) {
        throw std::logic_error("Invalid arguments to CellHolder");
    }
}

void Button::prepare() {
    glViewport(position.x, position.y, size.x, size.y);
}
