#include <iostream>
#include <set>
#include <cstdlib>
#include "shader.hpp"
#include "texture_loader.h"

#include <GL/glew.h>
#include <AntTweakBar.h>


#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/trigonometric.hpp>


#define WIN_WIDTH 1024
#define WIN_HEIGHT 768

#define POINTS_WIDTH WIN_WIDTH
#define POINTS_HEIGHT WIN_HEIGHT


GLFWwindow* window;
void init_glfw()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		throw std::logic_error("Failed to initialize GLFW\n");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Fractal", nullptr, nullptr);
	if (window == nullptr) {
		throw std::logic_error("Failed to open GLFW window. "
			"If you have an Intel GPU, "
			"they are not 3.3 compatible. "
			"Try the 2.1 version of the tutorials.\n");
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void init_glew()
{
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw std::logic_error("Failed to initialize GLEW");
	}
}

int main()
{
	init_glfw();
	init_glew();

    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    const size_t npts = POINTS_HEIGHT * POINTS_WIDTH;
    GLfloat *points = new GLfloat[3 * npts];

    for (int i = 0; i < npts; ++i) {
	    auto y = static_cast<GLfloat>(i / POINTS_WIDTH) / POINTS_HEIGHT;
	    auto x = static_cast<GLfloat>(i % POINTS_WIDTH) / POINTS_WIDTH;
        points[3*i + 0] = -1.0f + 2 * x;
        points[3*i + 1] = -1.0f + 2 * y;
        points[3*i + 2] = 0.0f;
    }

    // Create and compile our GLSL program from the shaders
    GLuint program_id = load_shaders("transform.vsh", "texture.fsh");

    // Get a handle for our "MVP" uniform
    GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

    GLuint iterations_id = glGetUniformLocation(program_id, "iterations");

    GLuint abs_lim_id = glGetUniformLocation(program_id, "abs_lim2");

    // Load texture.
    GLuint texture = GrayscaleTextureLoader(256).get_texture();

    // Get a handle for our "myTextureSampler" uniform
	// ReSharper disable once CppInconsistentNaming
    GLuint TextureID  = glGetUniformLocation(program_id, "myTextureSampler");

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * npts * sizeof(GLfloat), points, GL_STATIC_DRAW);

    // Use our shader
    glUseProgram(program_id);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        static_cast<void*>(nullptr)            // array buffer offset
    );

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

		const glm::mat4 projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
        const glm::mat4 view_matrix       = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), // eye
                                                 glm::vec3(0.0, 0.0, 0.0), // center
                                                 glm::vec3(0.0, 1.0, 0.0)  // up
                                                 );
		const glm::mat4 model_matrix = glm::mat4(1.0);
		const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
        glUniform1i(iterations_id, 100);
        glUniform1f(abs_lim_id, 10.0f);

        glDrawArrays(GL_POINTS, 0, npts);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glDisableVertexAttribArray(0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(program_id);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &vertex_array_id);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();


    return 0;
}
