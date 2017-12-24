#include "atb.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include "glfw_window_manager.h"
#include "points.h"
#include "gl_holder.h"

namespace atb {
	template<typename T>
	static void debug_point(const glm::tvec2<T> &p, const std::string &debug_string) {
		if (!debug_string.empty()) {
			std::cout << debug_string << " = ";
		}
		std::cout << p.x << ' ' << p.y << std::endl;
	}
	
	namespace impl {
		static TwBar *myBar;
		static GLFWWindowManager *window_manager;
		static PointTransformer *point_transfomer;
		//*  @param[in] window The window that received the event.
		//*  @param[in] button The[mouse button](@ref buttons) that was pressed or released.
		//*  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
		//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
		static void glfw_mouse_button_callback(GLFWwindow *, int button, int action, int)
		{
			TwEventMouseButtonGLFW(button, action);
		}

		static glm::dvec2 mouse_pos;
		static void glfw_mouse_pos_callback(GLFWwindow *_window, double xpos, double ypos)
		{
			if (!TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos)))
			{
				const glm::dvec2 cur_pos{ xpos, ypos };
				if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
				{
					point_transfomer->change_center_with_mouse_move(cur_pos - mouse_pos);
				}
			}
			mouse_pos = glm::vec2(xpos, ypos);
		}
		// TODO workaround when mouse lefts the screen.


		static GLfloat scale_delta_multiplier = 1.1f;
		//*  @param[in] window The window that received the event.
		//*  @param[in] xoffset The scroll offset along the x - axis.
		//*  @param[in] yoffset The scroll offset along the y - axis.
		static void glfw_mouse_wheel_callback(GLFWwindow *, double xoffset, double yoffset)
		{
			if (!TwEventMouseWheelGLFW(static_cast<int>(yoffset)))
			{
				assert(xoffset == 0.0);
				assert(scale_delta_multiplier > 1);
				
				const double multiplier = yoffset < 0 ? scale_delta_multiplier : 1 / scale_delta_multiplier;
				point_transfomer->multiply_scale(multiplier, mouse_pos);
			}
		}

		//*  @param[in] window The window that received the event.
		//*  @param[in] key The[keyboard key](@ref keys) that was pressed or released.
		//*  @param[in] scancode The system - specific scancode of the key.
		//*  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
		//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
		static void glfw_key_callback(GLFWwindow *_window, int key, int, int action, int)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			{
				glfwSetWindowShouldClose(_window, true);
				return;
			}

			TwEventKeyGLFW(key, action);
		}

		static void glfw_char_callback(GLFWwindow *, unsigned int input_char)
		{
			int glfw_action = GLFW_PRESS;
			TwEventCharGLFW(input_char, glfw_action);
		}


		static void glfw_resize_callback(GLFWwindow *, int width, int height)
		{
			window_manager->resize(width, height);

			TwWindowSize(width, height);
			glViewport(0, 0, width, height);
		}
	}

	void init(GLFWWindowManager *window_manager, PointTransformer *point_transformer, GLHolder *gl_holder)
	{
		impl::window_manager = window_manager;
		impl::point_transfomer = point_transformer;
		int tw_error;
		auto throw_on_atb_error = [&tw_error](const std::string &probable_cause)
		{
			if (tw_error == 0)
			{
				throw std::logic_error(probable_cause + ": " + TwGetLastError());
			}
		};
		tw_error = TwInit(TW_OPENGL_CORE, nullptr);
		throw_on_atb_error("TwInit");

		impl::myBar = TwNewBar("FractalTweakBar");
		tw_error = TwWindowSize(window_manager->win_width(), window_manager->win_height());
		throw_on_atb_error("TwWindowSize");

		tw_error = TwAddVarRW(impl::myBar, "iterations", TW_TYPE_UINT32, &gl_holder->iterations, "");
		throw_on_atb_error("TwAddVarRw(iterations)");

		tw_error = TwAddVarRW(impl::myBar, "abs_limit", TW_TYPE_FLOAT, &gl_holder->abs_lim, "");
		throw_on_atb_error("TwAddVarRw(AbsLimit)");

		tw_error = TwAddVarRW(impl::myBar, "scale", TW_TYPE_DOUBLE, &point_transformer->scale, " step=0.02 ");
		throw_on_atb_error("TwAddVarRw(scale)");
		
		tw_error = TwAddVarRW(impl::myBar, "center.x", TW_TYPE_DOUBLE, &point_transformer->center.x, " step=0.02 ");
		throw_on_atb_error("TwAddVarRw(center.x)");
		
		tw_error = TwAddVarRW(impl::myBar, "center.y", TW_TYPE_DOUBLE, &point_transformer->center.y, " step=0.02 ");
		throw_on_atb_error("TwAddVarRw(center.y)");

		GLFWwindow *window = window_manager->window();
		// after GLFW initialization
		// directly redirect GLFW events to AntTweakBar
		glfwSetMouseButtonCallback(window, static_cast<GLFWmousebuttonfun>(impl::glfw_mouse_button_callback));
		glfwSetCursorPosCallback(window, static_cast<GLFWcursorposfun>(impl::glfw_mouse_pos_callback));
		glfwSetScrollCallback(window, static_cast<GLFWscrollfun>(impl::glfw_mouse_wheel_callback));

		glfwSetKeyCallback(window, static_cast<GLFWkeyfun>(impl::glfw_key_callback));
		glfwSetCharCallback(window, static_cast<GLFWcharfun>(impl::glfw_char_callback));

		glfwSetWindowSizeCallback(window, static_cast<GLFWwindowsizefun>(impl::glfw_resize_callback));
	}

	void draw()
	{
		if (TwDraw() == 0)
		{
			throw std::logic_error(std::string("TwDraw: ") + TwGetLastError());
		}
	}
}
