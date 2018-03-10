#include <cassert>

#include "gl_holder.h"
#include "window_manager.h"
#include "color/ConstantColor.h"
#include "color/BlinkColor.h"

#define START_BUTTON_POS_X 320
#define START_BUTTON_POS_Y 100

#define START_BUTTON_WIDTH 400
#define START_BUTTON_HEIGHT 150

static const glm::vec4 BACKGROUND_COLOR{1, 0, 0, 1};

GLHolder::GLHolder(std::shared_ptr<WindowManager> window_manager)
        : window_manager(window_manager)
        , passthrough_pass(width(), height())
        , cells_pass(width(), height())
        , start_button_pass(
                width(), height(),
                glm::ivec2(START_BUTTON_POS_X, START_BUTTON_POS_Y),
                glm::ivec2(START_BUTTON_WIDTH, START_BUTTON_HEIGHT))
{
    cells_pass.set_fgcolor(std::make_shared<ConstantColor>(glm::vec4(0.5, 0, 0, 1)));
    cells_pass.set_bgcolor(std::make_shared<ConstantColor>(glm::vec4(1)));
    start_button_pass.set_bgcolor(std::make_shared<BlinkColor>(10, glm::vec4(0.5, 0, 0, 1), glm::vec4(0, 0.5, 0, 1)));
    start_button_pass.set_fgcolor(std::make_shared<ConstantColor>(glm::vec4(1)));
}

int GLHolder::width() const {
    return window_manager->win_width();
}

int GLHolder::height() const {
    return window_manager->win_height();
}

void GLHolder::paint() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());
    glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cells_pass.pass();
    start_button_pass.pass();
    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() = default;
