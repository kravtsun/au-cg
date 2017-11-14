#ifndef GLFW_WINDOW_MANAGER_H
#define GLFW_WINDOW_MANAGER_H

#include <functional>
#include <stdexcept>

struct GLFWwindow;

class GLFWWindowManager
{
public:
	GLFWWindowManager(const std::string &win_name, int win_width, int win_height);

	virtual ~GLFWWindowManager();

	std::string win_name() const { return win_name_; }
	int win_width() const { return win_width_; }
	int win_height() const { return win_height_; }
	GLFWwindow *window() const { return window_; }

	void main_loop(std::function<void()> inside_action) const;

	void resize(int new_width, int new_height);

private:
	std::string win_name_;
	int win_width_, win_height_;
	GLFWwindow *window_;
};
#endif // GLFW_WINDOW_MANAGER_H
