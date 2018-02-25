#include "gl_holder.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "texture_loader.h"
#include "shader.h"

GLHolder::GLHolder(PointTransformer *point_transformer) : point_transformer_(point_transformer)
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
	const auto &points = point_transformer_->get_points();
	auto npts = static_cast<const int>(points.size());

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * npts * sizeof(GLfloat), &points[0], GL_STREAM_DRAW);

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
	const glm::dvec2 &center = point_transformer_->center;
	glUniform2f(center_id, static_cast<GLfloat>(center.x), static_cast<GLfloat>(center.y));
	
	const glm::dvec2 &scale = point_transformer_->get_scale();
	glUniform1f(scale_x_id, static_cast<GLfloat>(scale.x));
	glUniform1f(scale_y_id, static_cast<GLfloat>(scale.y));

	glDrawArrays(GL_POINTS, 0, npts);
	glDisableVertexAttribArray(0);
}

GLHolder::~GLHolder()
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(program_id);
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vertex_array_id);
}
