#ifndef GLFW_WINDOW_MANAGER_H
#define GLFW_WINDOW_MANAGER_H

#include <functional>
#include <stdexcept>

#include "window_manager.h"

struct GLFWwindow;

struct GLFWWindowManager : WindowManager {
	GLFWWindowManager(const std::string &win_name, int win_width, int win_height);

	virtual ~GLFWWindowManager();

	GLFWwindow *window() const { return window_; }

	void main_loop(std::function<void()> inside_action) const;

	void resize(int new_width, int new_height);
private:
	GLFWwindow *window_;
};
#endif // GLFW_WINDOW_MANAGER_H
