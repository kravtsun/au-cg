#include <cassert>
#include <glm/glm.hpp>
#include "gl_holder.h"
#include "shader.h"
#include "scene.h"
#include "controls.h"

enum GBUFFER_TEXTURE_TYPE {
    GBUFFER_TEXTURE_TYPE_POSITION,
//    GBUFFER_TEXTURE_TYPE_DIFFUSE,
    GBUFFER_TEXTURE_TYPE_NORMAL,
//    GBUFFER_TEXTURE_TYPE_TEXCOORD,
    GBUFFER_NUM_TEXTURES
};
GLuint textures[GBUFFER_NUM_TEXTURES];
GLuint depth_texture;

static GLuint fbo;
//static GLuint program_id;
static GLuint debug_program_id;
static GLuint first_program_id, second_program_id;

static GLint mvp_id, m_id, v_id;

static GLuint vertex_array_id, vertexbuffer, normalbuffer;


static GLint texID;

static std::shared_ptr<Scene> scene;

static void init_gbuffer(const int width, const int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Создаем текстуры gbuffer
    glGenTextures(GBUFFER_NUM_TEXTURES, textures);
    glGenTextures(1, &depth_texture);
    
    for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES ; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    
        // Poor filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }
    
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
//            , GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount > 1);
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
    : window_manager(window_manager)
{
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    assert(glGetError() == GL_NO_ERROR);
    
    const auto width = window_manager->win_width();
    const auto height = window_manager->win_height();
    init_gbuffer(width, height);
    
//    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
//    glDepthFunc(GL_LESS);
//    glEnable(GL_CULL_FACE);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    first_program_id = load_shaders("first.vsh", "first.fsh");
//    second_program_id = load_shaders("second.vsh", "second.fsh");
//    program_id = load_shaders("transform.vsh", "texture.fsh");
    
    glUseProgram(first_program_id);
    mvp_id = glGetUniformLocation(first_program_id, "MVP");
    m_id = glGetUniformLocation(first_program_id, "M");
    v_id = glGetUniformLocation(first_program_id, "V");
    
//    scene = std::make_shared<Scene>("height_map.obj");
//    scene = std::make_shared<Scene>("../object/scene.obj");
    scene = std::make_shared<Scene>("sagar.obj");
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_vertices.size() * sizeof(glm::vec3), &Model::all_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_normals.size() * sizeof(glm::vec3), &Model::all_normals[0], GL_STATIC_DRAW);
    
    debug_program_id = load_shaders("Passthrough.vsh", "SimpleTexture.fsh");
    texID = glGetUniformLocation(debug_program_id, "renderedTexture");
}

void GLHolder::paint() {
    const int width = window_manager->win_width();
    const int height = window_manager->win_height();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(first_program_id);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            static_cast<void *>(nullptr) // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            static_cast<void *>(nullptr) // array buffer offset
    );

    computeMatricesFromInputs(window_manager->window());
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
//    PASS_UNIFORM_3F(camera_position_id, getCameraPosition());
    for (auto const &model : scene->models) {
        const auto &model_matrix = model->model_matrix;
        const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        glUniformMatrix4fv(mvp_id, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(m_id, 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(v_id, 1, GL_FALSE, &view_matrix[0][0]);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset), static_cast<GLsizei>(model->size()));
        model->step();
    }
    assert(glGetError() == GL_NO_ERROR);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(debug_program_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(texID, 0);
    
    GLuint quad_VertexArrayID;
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);

    const GLfloat quadz = 1.0f;
    static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, quadz,
            1.0f, -1.0f, quadz,
            -1.0f,  1.0f, quadz,
            -1.0f,  1.0f, quadz,
            1.0f, -1.0f, quadz,
            1.0f,  1.0f, quadz,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

    glDisableVertexAttribArray(0);

    assert(glGetError() == GL_NO_ERROR);
}

GLHolder::~GLHolder() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
//    glDeleteProgram(program_id);
    glDeleteProgram(first_program_id);
//    glDeleteProgram(second_program_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    
//    glDeleteProgram(depthProgramID);
}

