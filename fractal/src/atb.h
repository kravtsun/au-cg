#ifndef ATB_H
#define ATB_H

class GLFWWindowManager;
class GLHolder;

namespace atb
{
	void init(GLFWWindowManager *window_manager, GLHolder *gl_holder);

	void draw();
}
#endif // ATB_H
