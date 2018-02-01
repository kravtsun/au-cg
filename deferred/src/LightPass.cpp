#include <cassert>

#include "LightPass.h"
#include "Light.h"
#include "shader.h"
#include "controls.h"

void LightPass::init_light_render_buffer() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    init_output_texture(color_texture);
}

LightPass::LightPass(int width, int height)
        : Pass(width, height)
{
    init_light_render_buffer();
    program_id = load_shaders("PassthroughTexture.vsh", "light.fsh");
    position_texture_id = glGetUniformLocation(program_id, "positionTexture");
    normal_texture_id = glGetUniformLocation(program_id, "normalTexture");
    diffuse_texture_id = glGetUniformLocation(program_id, "diffuseTexture");
    ambient_texture_id = glGetUniformLocation(program_id, "ambientTexture");
    
    specular_color_id = glGetUniformLocation(program_id, "specular_color");
    v_id = glGetUniformLocation(program_id, "V");
    light_position_id = glGetUniformLocation(program_id, "lightPos");
    light_color_id = glGetUniformLocation(program_id, "lightColor");
    light_angle_id = glGetUniformLocation(program_id, "lightAngle");
}

// TODO migrate to unified LightPass::pass() and make it virtual with Pass::pass() = 0.
void LightPass::pass(const GeometryPass &geometryPass,
                     const TexturePass &texturePass,
                     const std::vector<std::shared_ptr<Light>> &lights)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, getWidth(), getHeight());
    
    int texId = mode == DEFERRED ? GBUFFER_TEXTURE_TYPE_AMBIENT : static_cast<int>(mode);
    auto texture = geometryPass.textures[texId];
    texturePass.pass(texture);
    
    if (mode == DEFERRED) {
        glUseProgram(program_id);
        const GLint texture_ids[] = {
                position_texture_id, normal_texture_id, diffuse_texture_id, ambient_texture_id
        };
        const int texture_ids_count = sizeof(texture_ids) / sizeof(texture_ids[0]);
        static_assert(texture_ids_count == GBUFFER_NUM_TEXTURES, "");
        for (int itex = 0; itex < texture_ids_count; ++itex) {
            glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + itex));
            glBindTexture(GL_TEXTURE_2D, geometryPass.textures[itex]);
            glUniform1i(texture_ids[itex], itex);
        }
        
        glUniform3f(specular_color_id, 1.0, 1.0f, 1.0f);
        
        auto v = getViewMatrix();
        PASS_UNIFORM_MAT4(v_id, v);
        
        const bool blendEnabledBefore = glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        
        for (auto const &light : lights) {
            PASS_UNIFORM_3F(light_position_id, light->getPosition());
            PASS_UNIFORM_3F(light_color_id, light->getColor());
            glUniform1f(light_angle_id, light->getAngle());
            drawTexture();
            light->step();
        }
        
        if (!blendEnabledBefore) {
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_BLEND);
        }
    }
}

LightPass::~LightPass() {
    glDeleteProgram(program_id);
    glDeleteTextures(1, &color_texture);
    glDeleteFramebuffers(1, &fbo);
}

GLuint LightPass::outputTexture() const {
    return color_texture;
}
