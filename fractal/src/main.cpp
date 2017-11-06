#include <iostream>
#include <set>
#include <cstdlib>
#include <cassert>
#include "shader.hpp"
#include "texture_loader.h"

#include <GL/glew.h>
#include <AntTweakBar.h>


#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int win_width = 1024, win_height = 768;
GLFWwindow* window;

void init_glfw()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		throw std::logic_error("Failed to initialize GLFW\n");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(win_width, win_height, "Fractal", nullptr, nullptr);
	
	if (window == nullptr) {
		throw std::logic_error("Failed to open GLFW window. "
			"If you have an Intel GPU, "
			"they are not 3.3 compatible. "
			"Try the 2.1 version of the tutorials.\n");
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void init_glew()
{
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		throw std::logic_error("Failed to initialize GLEW");
	}
}

GLuint program_id;
GLuint vertexbuffer;
GLuint matrix_id, iterations_id, abs_lim_id, center_id;
//GLuint scale_id;
GLuint scale_x_id, scale_y_id;
GLuint texture_id;
GLuint vertex_array_id;

void init_gl()
{
	// Create and compile our GLSL program from the shaders
	program_id = load_shaders("transform.vsh", "texture.fsh");

	// Get a handle for our "MVP" uniform
	matrix_id = glGetUniformLocation(program_id, "MVP");

	iterations_id = glGetUniformLocation(program_id, "iterations");

	abs_lim_id = glGetUniformLocation(program_id, "abs_lim2");

	center_id = glGetUniformLocation(program_id, "center");

	//scale_id = glGetUniformLocation(program_id, "scale");
	scale_x_id = glGetUniformLocation(program_id, "scale_x");
	scale_y_id = glGetUniformLocation(program_id, "scale_y");

	// Load texture.
	const GLuint texture = GrayscaleTextureLoader(256).get_texture();

	// Get a handle for our "myTextureSampler" uniform
	// ReSharper disable once CppInconsistentNaming
	texture_id = glGetUniformLocation(program_id, "myTextureSampler");

	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Use our shader
	glUseProgram(program_id);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(texture_id, 0);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
}

GLuint iterations = 100;
GLfloat abs_lim = 10.0f;

glm::vec2 center;
GLfloat scale = 1.0;
GLfloat scale_delta_multiplier = 1.1f;

static double get_scale_x()
{
	if (win_height > win_width)
	{
		return scale;
	}
	return scale * static_cast<double>(win_width) / win_height;
}

static double get_scale_y()
{
	if (win_height > win_width)
	{
		return scale * static_cast<double>(win_height) / win_width;
	}
	return scale;
}


static double get_x_delta()
{
	return 2.0 / win_width;
}

static double get_y_delta()
{
	return 2.0 / win_height;
}

void paint_gl()
{
	const GLsizei npts = win_width * win_height;
	GLfloat *points = new GLfloat[2 * npts];

	for (int y = 0; y < win_height; ++y)
	{
		for (int x = 0; x < win_width; ++x)
		{
			const int i = y * win_width + x;
			points[2 * i + 0] = -1.0f + x * get_x_delta();
			points[2 * i + 1] = -1.0f + y * get_y_delta();
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * npts * sizeof(GLfloat), points, GL_STREAM_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		static_cast<void*>(nullptr)            // array buffer offset
	);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
	const glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), // eye
		glm::vec3(0.0, 0.0, 0.0), // center
		glm::vec3(0.0, 1.0, 0.0)  // up
	);
	const glm::mat4 model_matrix = glm::mat4(1.0);
	const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
	glUniform1i(iterations_id, iterations);
	glUniform1f(abs_lim_id, abs_lim);
	glUniform2f(center_id, center.x, center.y);
	glUniform1f(scale_x_id, get_scale_x());
	glUniform1f(scale_y_id, get_scale_y());
	//glUniform1f(scale_id, scale);

	glDrawArrays(GL_POINTS, 0, npts);
	glDisableVertexAttribArray(0);
	delete[]points;
}

void deinit_gl()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(program_id);
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vertex_array_id);
}

glm::vec2 mouse_pos;

//*  @param[in] window The window that received the event.
//*  @param[in] button The[mouse button](@ref buttons) that was pressed or
//*released.
//*  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
GLFWmousebuttonfun default_mouse_button_callback;
void glfw_mouse_button_callback(GLFWwindow *_window, int button, int action, int mods)
{
	assert(_window == window);
	if (!TwEventMouseButtonGLFW(button, action))
	{
		//if (default_mouse_button_callback)
		//{
		//	default_mouse_button_callback(_window, button, action, mods);
		//}
		if (action == GLFW_PRESS)
		{
			
		}
		else if (action == GLFW_RELEASE)
		{
			
		}
		// TODO workaround when mouse lefts the screen.
	}
}

//*@param[in] window The window that received the event.
//*  @param[in] xpos The new cursor x - coordinate, relative to the left edge of
//*  the client area.
//*  @param[in] ypos The new cursor y - coordinate, relative to the top edge of the
//*  client area.
//*
//*  @sa @ref cursor_pos
//*  @sa glfwSetCursorPosCallback
//*
//*  @since Added in version 3.0.Replaces `GLFWmouseposfun`.
//*
//*  @ingroup input
//* /
//typedef void(*GLFWcursorposfun)(GLFWwindow*, double, double);
GLFWcursorposfun default_mouse_pos_callback;
void glfw_mouse_pos_callback(GLFWwindow *_window, double xpos, double ypos)
{
	assert(_window == window);
	if (!TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos)))
	{
		//if (default_mouse_pos_callback)
		//{
		//	default_mouse_pos_callback(_window, xpos, ypos);
		//}

		glm::vec2 cur_pos{ xpos, ypos };
		if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{
			auto pos_delta = cur_pos - mouse_pos;
			pos_delta.x *= -get_scale_x() * get_x_delta();
			pos_delta.y *= get_scale_y() * get_y_delta();
			center += pos_delta;
		}
	}
	mouse_pos = glm::vec2(static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
}

//*@param[in] window The window that received the event.
//*  @param[in] xoffset The scroll offset along the x - axis.
//*  @param[in] yoffset The scroll offset along the y - axis.
//*
//*  @sa @ref scrolling
//*  @sa glfwSetScrollCallback
//*
//*  @since Added in version 3.0.Replaces `GLFWmousewheelfun`.
//*
//*  @ingroup input
//* /
//typedef void(*GLFWscrollfun)(GLFWwindow*, double, double);
GLFWscrollfun default_mouse_wheel_callback;
void glfw_mouse_wheel_callback(GLFWwindow *_window, double xoffset, double yoffset)
{
	assert(_window == window);
	if (!TwEventMouseWheelGLFW(static_cast<int>(yoffset)))
	{
		//if (default_mouse_wheel_callback)
		//{
		//	default_mouse_wheel_callback(_window, xoffset, yoffset);
		//}
		assert(xoffset == 0.0);
		assert(scale_delta_multiplier > 1);
		if (yoffset < 0)
		{
			scale *= scale_delta_multiplier;
		}
		else if (yoffset > 0)
		{
			scale /= scale_delta_multiplier;
		}
	}
}

//*@param[in] window The window that received the event.
//*  @param[in] key The[keyboard key](@ref keys) that was pressed or released.
//*  @param[in] scancode The system - specific scancode of the key.
//*  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.
//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
//*  held down.
//*
//*  @sa @ref input_key
//*  @sa glfwSetKeyCallback
//*
//*  @since Added in version 1.0.
//*  @glfw3 Added window handle, scancode and modifier mask parameters.
//*
//*  @ingroup input
//* /
//typedef void(*GLFWkeyfun)(GLFWwindow*, int, int, int, int);
GLFWkeyfun default_key_callback;
void glfw_key_callback(GLFWwindow *_window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	assert(_window == window);
	if (!TwEventKeyGLFW(key, action))
	{
		if (default_key_callback)
		{
			default_key_callback(_window, key, scancode, action, mods);
		}
	}
}

//*@param[in] window The window that received the event.
//*  @param[in] codepoint The Unicode code point of the character.
//*
//*  @sa @ref input_char
//*  @sa glfwSetCharCallback
//*
//*  @since Added in version 2.4.
//*  @glfw3 Added window handle parameter.
//*
//*  @ingroup input
//* /
//typedef void(*GLFWcharfun)(GLFWwindow*, unsigned int);
GLFWcharfun default_char_callback;
void glfw_char_callback(GLFWwindow *_window, unsigned int input_char)
{
	assert(_window == window);
	int glfw_action = GLFW_PRESS;
	if (!TwEventCharGLFW(input_char, glfw_action))
	{
		if (default_char_callback)
		{
			default_char_callback(_window, input_char);
		}
	}
}


//*@param[in] window The window that was resized.
//*  @param[in] width The new width, in screen coordinates, of the window.
//*  @param[in] height The new height, in screen coordinates, of the window.
//*
//*  @sa @ref window_size
//*  @sa glfwSetWindowSizeCallback
//*
//*  @since Added in version 1.0.
//*  @glfw3 Added window handle parameter.
//*
//*  @ingroup window
//* /
//typedef void(*GLFWwindowsizefun)(GLFWwindow*, int, int);
GLFWwindowsizefun default_resize_callback;
void glfw_resize_callback(GLFWwindow* _window, int width, int height)
{
	assert(_window == window);
	::win_width = width;
	::win_height = height;
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
	//glfwGetWindowFrameSize();

	//glfwSetWindowSize(_window, width, height);
	if (default_resize_callback)
	{
		default_resize_callback(_window, width, height);
	}
}

void throw_on_atb_error(int tw_error, const std::string &probable_cause)
{
	if (tw_error == 0)
	{
		throw std::logic_error(probable_cause + ": " + TwGetLastError());
	}
}

TwBar *myBar;
void init_atb()
{
	int tw_error = TwInit(TW_OPENGL_CORE, nullptr);
	throw_on_atb_error(tw_error, "TwInit");

	myBar = TwNewBar("FractalTweakBar");
	tw_error = TwWindowSize(win_width, win_height);
	throw_on_atb_error(tw_error, "TwWindowSize");

	tw_error = TwAddVarRW(myBar, "iterations", TW_TYPE_UINT32, &iterations, "");
	throw_on_atb_error(tw_error, "TwAddVarRw(iterations)");

	tw_error = TwAddVarRW(myBar, "abs_limit", TW_TYPE_FLOAT, &abs_lim, "");
	throw_on_atb_error(tw_error, "TwAddVarRw(AbsLimit)");

	tw_error = TwAddVarRW(myBar, "scale", TW_TYPE_FLOAT, &scale, "");
	throw_on_atb_error(tw_error, "TwAddVarRO(scale)");

	// after GLFW initialization
	// directly redirect GLFW events to AntTweakBar
	glfwSetMouseButtonCallback(window, static_cast<GLFWmousebuttonfun>(glfw_mouse_button_callback));
	default_mouse_pos_callback = glfwSetCursorPosCallback(window, static_cast<GLFWcursorposfun>(glfw_mouse_pos_callback));
	default_mouse_wheel_callback = glfwSetScrollCallback(window, static_cast<GLFWscrollfun>(glfw_mouse_wheel_callback));

	glfwSetKeyCallback(window, static_cast<GLFWkeyfun>(glfw_key_callback));
	glfwSetCharCallback(window, static_cast<GLFWcharfun>(glfw_char_callback));

	// send window size events to AntTweakBar
	glfwSetWindowSizeCallback(window, static_cast<GLFWwindowsizefun>(glfw_resize_callback)); // and call TwWindowSize in the function MyResize
}

int main()
{
	init_glfw();
	try
	{
		init_glew();
		init_atb();
		init_gl();
	}
	catch(std::exception e)
	{
		std::cerr << e.what() << std::endl;
		glfwTerminate();
		return -1;
	}

	try
	{
		while (!glfwWindowShouldClose(window))
		{
			paint_gl();
			int twError = TwDraw();
			if (twError == 0)
			{
				throw std::logic_error(std::string("TwDraw: ") + TwGetLastError());
			}

			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		} // Check if the ESC key was pressed or the window was closed
	}
	catch(std::exception e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}

	TwTerminate();
	deinit_gl();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
