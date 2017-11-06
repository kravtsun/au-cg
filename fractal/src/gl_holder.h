#ifndef GL_HOLDER_H
#define GL_HOLDER_H

#include <GL/glew.h>
#include <glm/vec2.hpp>

class GLFWWindowManager;

class GLHolder
{
public:
	explicit GLHolder(GLFWWindowManager* window_manager);

	void paint() const;

	void change_center_with_mouse_move(glm::dvec2 pos_delta);

	virtual ~GLHolder();

public:
	// uniform variables are stored in properties.
	// TODO code more comfortable variable storage, i.e. in (name, type, reference) tuples.
	GLuint iterations = 100;
	GLfloat abs_lim = 10.0f;
	GLfloat scale = 1.0;
	
private:
	GLFWWindowManager *window_manager_;

	GLuint program_id;
	GLuint vertexbuffer;
	GLuint matrix_id, iterations_id, abs_lim_id, center_id;
	GLuint scale_x_id, scale_y_id;
	GLuint texture_id;
	GLuint vertex_array_id;

	glm::vec2 center;

	double get_scale_x() const;

	double get_scale_y() const;

	double get_x_delta() const;

	double get_y_delta() const;
};
#endif // GL_HOLDER_H
