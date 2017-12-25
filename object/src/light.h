#ifndef OBJECT_LIGHT_H
#define OBJECT_LIGHT_H

#include <cmath>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

struct Model;

struct Light {
    Light(GLuint program_id,
          const char *shadowMapName, const char *depthBiasVPName, const char *lightPosName, const char *lightColorName,
          GLuint depthProgramID, const char *depthMVPName);
    
    virtual void step();
    
    glm::vec3 getPosition() const {
        return position;
    }
    
    glm::vec3 getColor() const {
        return glm::vec3(1.0, 1.0, 1.0);
    }
    
    glm::vec3 getInvDir() const {
        return getPosition();
    }

    virtual ~Light() {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &depthTexture);
    }
    
    void paint(GLuint vertexbuffer, const std::vector<std::shared_ptr<Model>> &models);
    
private:
    void update_matrices();
    
    void initFramebuffer();
    
public:
    const GLuint program_id;
    GLint position_id, color_id;
    GLint ShadowMapID, DepthBiasID;
    
    const GLuint depthProgramID;
    glm::mat4 depthProjectionMatrix;
    glm::mat4 depthViewMatrix;
    
    GLuint depthTexture;
    GLint depthMatrixID;
    
    glm::mat4 getDepthBiasVP() const {
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix;
        const glm::mat4 biasMatrix(
                0.5, 0.0, 0.0, 0.0,
                0.0, 0.5, 0.0, 0.0,
                0.0, 0.0, 0.5, 0.0,
                0.5, 0.5, 0.5, 1.0
        );
        return biasMatrix * depthMVP;
    }
    
private:
    glm::vec3 position = glm::vec3(4, 4, 4);
    double angle = 0.0;
    GLuint framebuffer = 0;
};

#endif //OBJECT_LIGHT_H
