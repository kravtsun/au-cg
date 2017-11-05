#include <iostream>
#include <set>
#include "shader.hpp"
#include "texture_loader.h"

#define GLEW_STATIC
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


int main() {

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow( WIN_WIDTH, WIN_HEIGHT, "Fractal", NULL, NULL);
    if( window == NULL ) {
        fprintf( stderr, "Failed to open GLFW window. "
                         "If you have an Intel GPU, "
                         "they are not 3.3 compatible. "
                         "Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    const size_t npts = POINTS_HEIGHT * POINTS_WIDTH;
    GLfloat *points = new GLfloat[3 * npts];

    std::set<GLfloat> yy;
    for (int i = 0; i < npts; ++i) {
        GLfloat y = (GLfloat)(i / POINTS_WIDTH) / POINTS_HEIGHT;
        GLfloat x = (GLfloat)(i % POINTS_WIDTH) / POINTS_WIDTH;
        yy.insert(y);
        points[3*i + 0] = -1.0f + 2 * x;
        points[3*i + 1] = -1.0f + 2 * y;
        points[3*i + 2] = 0.0f;
    }

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("transform.vsh", "texture.fsh");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    GLuint IterationsID = glGetUniformLocation(programID, "iterations");

    GLuint AbsLimID = glGetUniformLocation(programID, "abs_lim2");

    // Load texture.
    GLuint Texture = GrayscaleTextureLoader(256).getTexture();

    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * npts * sizeof(GLfloat), points, GL_STATIC_DRAW);

    // Use our shader
    glUseProgram(programID);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, Texture);
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
        (void*)0            // array buffer offset
    );

    int i = 0;
    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 ProjectionMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 2.0f);
        glm::mat4 ViewMatrix       = glm::lookAt(glm::vec3(0.0, 0.0, 1.0), // eye
                                                 glm::vec3(0.0, 0.0, 0.0), // center
                                                 glm::vec3(0.0, 1.0, 0.0)  // up
                                                 );
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform1i(IterationsID, 100);
        glUniform1f(AbsLimID, 10.0f);

        //std::cout << i++ << std::endl;

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
    glDeleteProgram(programID);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();


    return 0;
}
