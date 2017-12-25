#ifndef OBJECT_LIGHT_H
#define OBJECT_LIGHT_H

#include <cmath>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Light {
    explicit Light(GLuint program_id)
            : program_id(program_id)
    {
        glUseProgram(program_id);
        position_id = glGetUniformLocation(program_id, "lightPos");
        color_id = glGetUniformLocation(program_id, "lightColor");
        step();
        
    }
    
    void step() {
        angle += 0.01;
        position.x = static_cast<float>(4 * cos(angle));
        position.z = static_cast<float>(4 * sin(angle));
        
        // Compute the MVP matrix from the light's point of view
        depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
        depthViewMatrix = glm::lookAt(getPosition(), glm::vec3(0,0,0), glm::vec3(0,1,0));
        
        // or, for spot light :
        //glm::vec3 lightPos(5, 20, 20);
//        depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
//        depthViewMatrix = glm::lookAt(getPosition(), getPosition() - getInvDir(), glm::vec3(0, 1, 0));
    }
    
    glm::vec3 getPosition() const {
        return position;
    }
    
    glm::vec3 getColor() const {
        return glm::vec3(1.0, 1.0, 1.0);
    }
    
    glm::vec3 getInvDir() const {
        return getPosition();
    }
    
    // TODO make static.
    GLint position_id, color_id;
    const GLuint program_id;
    
    glm::mat4 depthProjectionMatrix;
    glm::mat4 depthViewMatrix;
    
    glm::vec3 position = glm::vec3(4, 4, 4);
    double angle = 0.0;
};

#endif //OBJECT_LIGHT_H
