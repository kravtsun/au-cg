#include <cassert>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glut_window_manager.h"

static std::function<void()> inside_action;

static void display()
{
    inside_action();
    glutSwapBuffers();
}

static void idle() {
    glutPostRedisplay();
}

GLUTWindowManager::GLUTWindowManager(const std::string &win_name, int win_width, int win_height, int argc, char **argv)
                                : WindowManager(win_name, win_width, win_height)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(win_width, win_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(win_name.c_str());
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glewExperimental = GL_TRUE;
    glewInit();
    volatile auto gl_error = glGetError();
    (void)gl_error;
}


void GLUTWindowManager::main_loop(std::function<void()> inside_action) const {
    ::inside_action = inside_action;
    glutMainLoop();
}
