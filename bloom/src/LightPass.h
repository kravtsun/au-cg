#ifndef DEFERRED_LIGHTPASS_H
#define DEFERRED_LIGHTPASS_H

#include "PassCommon.h"
#include "TexturePass.h"
#include "GeometryPass.h"

struct Light;

struct LightPass : public Pass {
    GLuint fbo;
    GLuint program_id;
    GLint position_texture_id, normal_texture_id;
    GLint diffuse_texture_id, ambient_texture_id;
    
    GLuint color_texture;
    
    GLint specular_color_id;
    GLint v_id;
    GLint light_position_id, light_color_id, light_angle_id;
    
public:
    LightPass(int width, int height);
    
    void pass(const GLuint *geometryTextures,
              const TexturePass &texturePass,
              const std::vector<std::shared_ptr<Light>> &lights);
    
    GLuint outputTexture() const override;
    
    virtual ~LightPass();
private:
    void init_light_render_buffer();
};


#endif //DEFERRED_LIGHTPASS_H
