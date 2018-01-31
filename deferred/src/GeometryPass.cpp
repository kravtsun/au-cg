#include <cassert>

#include "GeometryPass.h"
#include "scene.h"
#include "controls.h"
#include "shader.h"

void GeometryPass::init_gbuffer() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    // Создаем текстуры gbuffer
    glGenTextures(GBUFFER_NUM_TEXTURES, textures);
    glGenTextures(1, &depth_texture);
    
    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        init_texture(getWidth(), getHeight(), textures[i], GL_COLOR_ATTACHMENT0 + i);
    }

//    init_texture(width, height, depth_texture, GL_DEPTH_ATTACHMENT);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getWidth(), getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    
    const GLenum DrawBuffers[] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
            , GL_COLOR_ATTACHMENT2
            , GL_COLOR_ATTACHMENT3
    };
    const GLsizei drawBuffersCount = sizeof(DrawBuffers) / sizeof(DrawBuffers[0]);
    static_assert(drawBuffersCount == GBUFFER_NUM_TEXTURES, "drawBuffersCount == GBUFFER_NUM_TEXTURES");
    glDrawBuffers(drawBuffersCount, DrawBuffers);
    
    const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(fbo_status == GL_FRAMEBUFFER_COMPLETE);
}

GeometryPass::GeometryPass(int width, int height)
    : Pass(width, height)
{
    init_gbuffer();
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    
    program_id = load_shaders("geom.vsh", "geom.fsh");
    
    glUseProgram(program_id);
    mvp_id = glGetUniformLocation(program_id, "MVP");
    m_id = glGetUniformLocation(program_id, "M");
    v_id = glGetUniformLocation(program_id, "V");
    diffuse_color_id = glGetUniformLocation(program_id, "diffuse_color");
    ambient_color_id = glGetUniformLocation(program_id, "ambient_color");
//    vertex_color_id = glGetUniformLocation(program_id, "vertexColor");
}

void GeometryPass::load_scene(const std::string &scene_path) {
    scene = std::make_shared<Scene>(scene_path);
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_vertices.size() * sizeof(glm::vec3), &Model::all_vertices[0],
                 GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, Model::all_normals.size() * sizeof(glm::vec3), &Model::all_normals[0],
                 GL_STATIC_DRAW);
}

void GeometryPass::pass() {
    // TODO 3 those repeating lines move tgo PassCommon::pass
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, getWidth(), getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const bool depthTestEnabledBefore = glIsEnabled(GL_DEPTH_TEST);
    if (!depthTestEnabledBefore) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    
    glUseProgram(program_id);
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
    
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
//    PASS_UNIFORM_3F(camera_position_id, getCameraPosition());
    for (auto const &model : scene->models) {
        const auto &model_matrix = model->model_matrix;
        const glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        PASS_UNIFORM_MAT4(mvp_id, mvp);
        PASS_UNIFORM_MAT4(m_id, model_matrix);
        PASS_UNIFORM_MAT4(v_id, view_matrix);
        
        glUniform3f(diffuse_color_id, 1.0, 0.0f, 0.0f);
        glUniform3f(ambient_color_id, 0.0, 0.0f, 0.0f);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset), static_cast<GLsizei>(model->size()));
        model->step();
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    if (!depthTestEnabledBefore) {
        glDisable(GL_DEPTH_TEST);
    }
}

GeometryPass::~GeometryPass() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteVertexArrays(1, &vertex_array_id);
    glDeleteProgram(program_id);
    glDeleteTextures(GBUFFER_NUM_TEXTURES, textures);
    glDeleteFramebuffers(1, &fbo);
}
