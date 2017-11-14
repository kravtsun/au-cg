#ifndef ATB_H
#define ATB_H


class GLFWWindowManager;
class GLHolder;
class PointTransformer;

namespace atb
{
	void init(GLFWWindowManager *window_manager, PointTransformer *point_transformer, GLHolder *gl_holder);

	void draw();
}
#endif // ATB_H
