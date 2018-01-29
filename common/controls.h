#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void changeFov(float newFov);

void computeMatricesFromInputs(GLFWwindow *window);

glm::mat4 getViewMatrix();

glm::mat4 getProjectionMatrix();

glm::vec3 getCameraPosition();

#endif
