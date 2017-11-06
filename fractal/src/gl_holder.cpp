#include "gl_holder.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "glfw_window_manager.h"
#include "texture_loader.h"
#include "shader.hpp"

GLHolder::GLHolder(GLFWWindowManager* window_manager) : window_manager_(window_manager)
{
	// Create and compile our GLSL program from the shaders
	program_id = load_shaders("transform.vsh", "texture.fsh");

	// Get a handle for our "MVP" uniform
	matrix_id = glGetUniformLocation(program_id, "MVP");

	iterations_id = glGetUniformLocation(program_id, "iterations");

	abs_lim_id = glGetUniformLocation(program_id, "abs_lim2");

	center_id = glGetUniformLocation(program_id, "center");

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

void GLHolder::paint() const
{
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	const GLsizei npts = win_width * win_height;
	GLfloat* points = new GLfloat[2 * npts];

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
		0, // attribute. No particular reason for 0, but must match the layout in the shader.
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		static_cast<void*>(nullptr) // array buffer offset
	);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	const glm::mat4 projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
	const glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), // eye
		glm::vec3(0.0, 0.0, 0.0), // center
		glm::vec3(0.0, 1.0, 0.0) // up
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

	glDrawArrays(GL_POINTS, 0, npts);
	glDisableVertexAttribArray(0);
	delete[]points;
}

void GLHolder::change_center_with_mouse_move(glm::dvec2 pos_delta)
{
	pos_delta.x *= -get_scale_x() * get_x_delta();
	pos_delta.y *= get_scale_y() * get_y_delta();
	center += pos_delta;
}

GLHolder::~GLHolder()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(program_id);
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vertex_array_id);
}

inline double GLHolder::get_scale_x() const
{
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	if (win_height > win_width)
	{
		return scale;
	}
	return scale * static_cast<double>(win_width) / win_height;
}

inline double GLHolder::get_scale_y() const
{
	const int win_height = window_manager_->win_height();
	const int win_width = window_manager_->win_width();
	if (win_height > win_width)
	{
		return scale * static_cast<double>(win_height) / win_width;
	}
	return scale;
}

inline double GLHolder::get_x_delta() const
{
	return 2.0 / window_manager_->win_width();
}

inline double GLHolder::get_y_delta() const
{
	return 2.0 / window_manager_->win_height();
}
