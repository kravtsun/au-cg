#ifndef GL_HOLDER_H
#define GL_HOLDER_H

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include "points.h"

class GLFWWindowManager;

class GLHolder
{
public:
	explicit GLHolder(PointTransformer *point_transformer);

	void paint() const;

	virtual ~GLHolder();
	
public:
	// uniform variables are stored in properties.
	// TODO code more comfortable variable storage, i.e. in (name, type, reference) tuples.
	GLuint iterations = 100;
	GLfloat abs_lim = 10.0f;
	
private:
	PointTransformer *point_transformer_;
	
	GLuint program_id;
	GLuint vertexbuffer;
	GLint matrix_id, iterations_id, abs_lim_id, center_id;
	GLint scale_x_id, scale_y_id;
	GLuint texture_id;
	GLuint vertex_array_id;
};
#endif // GL_HOLDER_H
