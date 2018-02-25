#include "controls.h"
#include "glfw_window_manager.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static glm::mat4 ViewMatrix;
static glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

// Initial position : on +Z
//glm::vec3 position = glm::vec3(0, 0, 5);
//horizontalAngle = 2.346575
//verticalAngle = -0.100000
//static glm::vec3 position = glm::vec3(0, 2, 7);
static glm::vec3 position = glm::vec3(-5.206410, 4.134450, 4.195565);

glm::vec3 getCameraPosition() {
    return position;
}

// Initial horizontal angle : toward -Z
//float horizontalAngle = static_cast<float>(M_PI);
float horizontalAngle = static_cast<float>(2.196572);
//float horizontalAngle = 2.342062;
// Initial vertical angle : none
float verticalAngle = -0.40000;
//float verticalAngle = -0.554490;

// Initial Field of View
static float FoV = 45.0f;

float move_speed = 3.0f; // 3 units / second
static const float rotate_speed = 0.005f;

void changeFov(float FoVdelta) {
    FoV -= FoVdelta;
}

void computeMatricesFromInputs(GLFWwindow *window) {
    static int cnt = 0;
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        verticalAngle += rotate_speed;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        verticalAngle -= rotate_speed;
    }
    
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngle += rotate_speed;
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngle -= rotate_speed;
    }
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    const glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
    );
    
    // Right vector
    const glm::vec3 right = glm::vec3(
            sin(horizontalAngle - M_PI / 2),
            0,
            cos(horizontalAngle - M_PI / 2)
    );
    
    // Up vector
    const glm::vec3 up = glm::cross(right, direction);
    
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        position += up * deltaTime * move_speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        position -= up * deltaTime * move_speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        position += right * deltaTime * move_speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        position -= right * deltaTime * move_speed;
    }
    
//    cnt++;
//    if (cnt % 10 == 0) {
//        printf("position = (%lf, %lf, %lf)\n", position.x, position.y, position.z);
//        printf("horizontalAngle = %lf\n", horizontalAngle);
//        printf("verticalAngle = %lf\n", verticalAngle);
//    }
//	float FoV = FoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    
    // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix = glm::lookAt(
            position,             // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                    // Head is up (set to 0,-1,0 to look upside-down)
    );
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
