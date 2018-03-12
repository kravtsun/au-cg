#include <iostream>
#include <cassert>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glut_window_manager.h"

static GLUTWindowManager::action_t main_loop_action;
static std::function<void(int, int, int, int)> mouse_action;

static void display()
{
    if (main_loop_action) {
        main_loop_action();
    }
    glutSwapBuffers();
}

static void mouse_callback(int button, int state, int x, int y) {
    if (mouse_action) {
        mouse_action(button, state, x, y);
    } else {
        std::cerr << "Warning: mouse action not set." << std::endl;
    }
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
    glutMouseFunc(mouse_callback);
    glutIdleFunc(idle);
    glewExperimental = GL_TRUE;
    glewInit();
    volatile auto gl_error = glGetError();
    (void)gl_error;
}


void GLUTWindowManager::main_loop(action_t main_loop_action) const {
    ::main_loop_action = main_loop_action;
    glutMainLoop();
}

void GLUTWindowManager::set_mouse_action(mouse_action_t mouse_action) const {
    ::mouse_action = mouse_action;
}
