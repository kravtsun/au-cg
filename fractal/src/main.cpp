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

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768

#define POINTS_WIDTH WIN_WIDTH
#define POINTS_HEIGHT WIN_HEIGHT

#define NPOINTS (POINTS_WIDTH * POINTS_HEIGHT)


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
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Fractal", nullptr, nullptr);
	if (window == nullptr) {
		throw std::logic_error("Failed to open GLFW window. "
			"If you have an Intel GPU, "
			"they are not 3.3 compatible. "
			"Try the 2.1 version of the tutorials.\n");
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
GLuint matrix_id, iterations_id, abs_lim_id;
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

	// Load texture.
	const GLuint texture = GrayscaleTextureLoader(256).get_texture();

	// Get a handle for our "myTextureSampler" uniform
	// ReSharper disable once CppInconsistentNaming
	texture_id = glGetUniformLocation(program_id, "myTextureSampler");

	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	GLfloat *points = new GLfloat[3 * NPOINTS];

	for (int i = 0; i < NPOINTS; ++i) {
		auto y = static_cast<GLfloat>(i / POINTS_WIDTH) / POINTS_HEIGHT;
		auto x = static_cast<GLfloat>(i % POINTS_WIDTH) / POINTS_WIDTH;
		points[3 * i + 0] = -1.0f + 2 * x;
		points[3 * i + 1] = -1.0f + 2 * y;
		points[3 * i + 2] = 0.0f;
	}

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * NPOINTS * sizeof(GLfloat), points, GL_STATIC_DRAW);

	// Use our shader
	glUseProgram(program_id);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(texture_id, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		static_cast<void*>(nullptr)            // array buffer offset
	);
}

GLuint iterations = 100;
GLfloat abs_lim = 10.0f;
void paint_gl()
{
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

	glDrawArrays(GL_POINTS, 0, NPOINTS);
}

void deinit_gl()
{
	glDisableVertexAttribArray(0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(program_id);
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vertex_array_id);
}

//*  @param[in] window The window that received the event.
//*  @param[in] button The[mouse button](@ref buttons) that was pressed or
//*released.
//*  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
//*  @param[in] mods Bit field describing which[modifier keys](@ref mods) were
void glfw_mouse_button_callback(GLFWwindow *_window, int button, int action, int mods)
{
	assert(_window == window);
	if (!TwEventMouseButtonGLFW(button, action))
	{
		return;
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
void glfw_mouse_pos_callback(GLFWwindow *_window, double xpos, double ypos)
{
	assert(_window == window);
	if (!TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos)))
	{
		return;
	}
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
void glfw_mouse_wheel_callback(GLFWwindow *_window, double xoffset, double yoffset)
{
	assert(_window == window);
	//if (!TwEventMouseWheelGLFW(int pos))
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
void glfw_key_callback(GLFWwindow *_window, int key, int scancode, int action, int mods)
{
	assert(_window == window);
	if (!TwEventKeyGLFW(key, action))
	{
		return;
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
void glfw_char_callback(GLFWwindow *_window, unsigned int input_char)
{
	assert(_window == window);
	int glfw_action = GLFW_PRESS;
	if (!TwEventCharGLFW(input_char, glfw_action))
	{
		return;
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
void glfw_resize(GLFWwindow* _window, int width, int height)
{
	assert(_window == window);
	//glfwGetWindowSize(window, &width, &height);
	TwWindowSize(width, height);
}

TwBar *myBar;
void init_atb()
{
	int tw_error = TwInit(TW_OPENGL_CORE, NULL);
	if (tw_error == 0)
	{
		throw std::logic_error(std::string("TwInit: ") + TwGetLastError());
	}
	
	myBar = TwNewBar("FractalTweakBar");
	tw_error = TwWindowSize(WIN_WIDTH, WIN_HEIGHT);
	if (tw_error == 0)
	{
		throw std::logic_error(std::string("TwWindowSize: ") + TwGetLastError());
	}

	tw_error = TwAddVarRW(myBar, "iterations", TW_TYPE_UINT32, &iterations, "");

	if (tw_error == 0)
	{
		throw std::logic_error(std::string("TwAddVarRW: ") + TwGetLastError());
	}

	// after GLFW initialization
	// directly redirect GLFW events to AntTweakBar
	glfwSetMouseButtonCallback(window, static_cast<GLFWmousebuttonfun>(glfw_mouse_button_callback));
	glfwSetCursorPosCallback(window, static_cast<GLFWcursorposfun>(glfw_mouse_pos_callback));
	glfwSetScrollCallback(window, static_cast<GLFWscrollfun>(glfw_mouse_wheel_callback));

	glfwSetKeyCallback(window, static_cast<GLFWkeyfun>(glfw_key_callback));
	glfwSetCharCallback(window, static_cast<GLFWcharfun>(glfw_char_callback));

	// send window size events to AntTweakBar
	glfwSetWindowSizeCallback(window, static_cast<GLFWwindowsizefun>(glfw_resize)); // and call TwWindowSize in the function MyResize
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
	catch(...)
	{
		glfwTerminate();
		return -1;
	}

	try
	{
		do
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
		while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
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
