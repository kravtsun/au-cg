#include <stdexcept>
#include <memory>
#include <iostream>
#include "light.h"
#include "model.h"

Light::Light(GLuint program_id,
             const char *shadowMapName,
             const char *depthBiasVPName,
             const char *lightPosName,
             const char *lightColorName,
             GLuint depthProgramID, const char *depthMVPName)
        : program_id(program_id)
        , depthProgramID(depthProgramID)
{
    glUseProgram(program_id);
    ShadowMapID = glGetUniformLocation(program_id, shadowMapName);
    DepthBiasID = glGetUniformLocation(program_id, depthBiasVPName);
    position_id = glGetUniformLocation(program_id, lightPosName);
    color_id = glGetUniformLocation(program_id, lightColorName);

    glUseProgram(depthProgramID);
    depthMatrixID = glGetUniformLocation(depthProgramID, depthMVPName);
    
    initFramebuffer();
    
    step();
}


void Light::step() {
    static bool first = true;
//    angle += 0.01;
    position.x = static_cast<float>(4 * cos(angle));
    position.z = static_cast<float>(4 * sin(angle));
    if (first) {
        first = false;
        std::cout << "Light: " << "(" << position.x << ", " << position.y << ")" << std::endl;
    }
    update_matrices();
}

void Light::update_matrices() {
    // Compute the MVP matrix from the light's point of view
    depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
    depthViewMatrix = glm::lookAt(getPosition(), glm::vec3(0,0,0), glm::vec3(0,1,0));
    
    // or, for spot light :
    //glm::vec3 lightPos(5, 20, 20);
//        depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
//        depthViewMatrix = glm::lookAt(getPosition(), getPosition() - getInvDir(), glm::vec3(0, 1, 0));
}

void Light::initFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    
    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::logic_error("framebuffer is not ok.");
    }
}

void Light::paint(GLuint vertexbuffer, const std::vector<std::shared_ptr<Model>> &models) {
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    // We don't use bias in the shader, but instead we draw back faces,
    // which are already separated from the front faces by a small distance
    // (if your geometry is made this way)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(depthProgramID);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,  // The attribute we want to configure
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr             // array buffer offset
    );
    
    for (auto const &model : models) {
        glm::mat4 model_matrix = model->model_matrix;
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * model_matrix;
        
        glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset), static_cast<GLsizei>(model->size()));
    }
    
    glDisableVertexAttribArray(0);
    
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, 1024, 768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
}
