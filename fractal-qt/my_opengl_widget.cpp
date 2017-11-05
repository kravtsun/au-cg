#include "my_opengl_widget.h"

//#include <QOpenGLVertexArrayObject>
//#include <qopengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "texture_loader.h"

#define POINTS_WIDTH 600
#define POINTS_HEIGHT 600

MyOpenGLWidget::MyOpenGLWidget()
    :ctx(new QOpenGLContext)
    ,fmt(new QSurfaceFormat)
{
//    setSurfaceType(QOpenGLSurface);
    fmt->setRenderableType(QSurfaceFormat::OpenGL);
    fmt->setVersion(3,3);

    fmt->setProfile(QSurfaceFormat::CoreProfile);
    setFormat(*fmt);
    ctx->setFormat(*fmt);
    ctx->create();

//    ctx->makeCurrent(this);
    initializeOpenGLFunctions();

//    initializeOpenGLFunctions();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    programID = ShaderLoader(this).LoadShaders("transform.vsh", "texture.fsh");

    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");

    IterationsID = glGetUniformLocation(programID, "iterations");

    AbsLimID = glGetUniformLocation(programID, "abs_lim2");

    // Load texture.
    GLuint Texture = GrayscaleTextureLoader(256).getTexture();

    // Get a handle for our "myTextureSampler" uniform
    TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    // Use our shader
    glUseProgram(programID);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);
}

void MyOpenGLWidget::paintGL() {
    const size_t npts = POINTS_HEIGHT * POINTS_WIDTH;
    GLfloat *points = new GLfloat[3 * npts];

    for (size_t i = 0; i < npts; ++i) {
        GLfloat y = (GLfloat)(i / POINTS_WIDTH) / POINTS_HEIGHT;
        GLfloat x = (GLfloat)(i % POINTS_WIDTH) / POINTS_WIDTH;
        points[3*i + 0] = -1.0f + 2 * x;
        points[3*i + 1] = -1.0f + 2 * y;
        points[3*i + 2] = 0.0f;
    }

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * npts * sizeof(GLfloat), points, GL_STATIC_DRAW);
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

    glDrawArrays(GL_POINTS, 0, npts);
    glDisableVertexAttribArray(0);
}

void MyOpenGLWidget::resizeGL(int w, int h) {
    paintGL();
}

MyOpenGLWidget::~MyOpenGLWidget() {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &VertexArrayID);
}
