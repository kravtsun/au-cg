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
    Light(
//            GLuint program_id,
//        const char *shadowMapName, const char *depthBiasVPName, const char *lightPosName, const char *lightColorName,
        GLuint depthProgramID, const char *depthMVPName,
        const glm::vec3 &position = glm::vec3(4, 4, 4));
    
    virtual void step() {
        update_matrices();
    }
    
    const glm::vec3 getPosition() const {
        return position;
    }
    
    const glm::vec3 getColor() const {
        return glm::vec3(1.0, 1.0, 1.0);
    }
    
    virtual ~Light() {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &depthTexture);
    }
    
    void preprocess_painting(GLuint vertexbuffer, const std::vector<std::shared_ptr<Model>> &models);

private:
    void init_framebuffer();
    
public:
//    const GLuint program_id;
//    GLint position_id, color_id;
//    GLint ShadowMapID, DepthBiasID;
    
    const GLuint depthProgramID;
    glm::mat4 depthProjectionMatrix;
    glm::mat4 depthViewMatrix;
    
    GLuint depthTexture;
    GLint depthMatrixID;
    
    const glm::mat4 getDepthBiasVP() const {
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
    GLuint framebuffer = 0;
    void update_matrices();

protected:
    glm::vec3 position;
};

struct CircularMovingLight : public Light {
    CircularMovingLight(GLuint depthProgramID, const char *depthMVPName, const glm::vec3 &position);
    CircularMovingLight(GLuint depthProgramID, const char *depthMVPName);
    
    void step() override;

private:
    double angle = 0.0;
};

#endif //OBJECT_LIGHT_H
