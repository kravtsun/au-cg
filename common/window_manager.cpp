#include "window_manager.h"

WindowManager::WindowManager(const std::string &win_name, int win_width, int win_height)
        : win_name_(win_name), win_width_(win_width), win_height_(win_height)
{}

void WindowManager::resize(int new_width, int new_height) {
    win_width_ = new_width;
    win_height_ = new_height;
}

WindowManager::~WindowManager() = default;
