#include "controls.hpp"

#include "glfw_window_manager.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}


// Initial position : on +Z
//glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 position = glm::vec3(-4.362360, 1.444663, 5);

glm::vec3 getCameraPosition() {
    return position;
}

// Initial horizontal angle : toward -Z
//float horizontalAngle = static_cast<float>(M_PI);
float horizontalAngle = 2.412064;
// Initial vertical angle : none
//float verticalAngle = 0;
float verticalAngle = 0.040510;

// Initial Field of View
float initialFoV = 45.0f;

float move_speed = 3.0f; // 3 units / second
float rotate_speed = 0.005f;

void computeMatricesFromInputs(GLFWwindow *window) {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        verticalAngle += rotate_speed;
    }
    
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        verticalAngle -= rotate_speed;
    }
    
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
        horizontalAngle += rotate_speed;
    }
    
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
        horizontalAngle -= rotate_speed;
    }
    
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - M_PI / 2),
		0,
		cos(horizontalAngle - M_PI /2 )
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += up * deltaTime * move_speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= up * deltaTime * move_speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * move_speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * move_speed;
	}
    
//    printf("position = (%lf, %lf, %lf)\n", position.x, position.y, position.z);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,             // Camera is here
								position + direction, // and looks here : at the same position, plus "direction"
								up                    // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
