#include "StartButton.h"
#include "wrappers/Texture.h"
#include "resources.h"

StartButton::StartButton(const glm::ivec2 &position, const glm::ivec2 &size)
        : Button(position, size)
        , texture(new Texture(START_BUTTON_TEXT_PATH))
{}
