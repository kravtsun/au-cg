#ifndef DEFERRED_GEOMETRYPASS_H
#define DEFERRED_GEOMETRYPASS_H

#include <memory>
#include <string>
#include "PassCommon.h"
#include "scene.h"

enum GBUFFER_TEXTURE_TYPE {
    GBUFFER_TEXTURE_TYPE_POSITION = 0,
    GBUFFER_TEXTURE_TYPE_NORMAL,
    GBUFFER_TEXTURE_TYPE_DIFFUSE,
    GBUFFER_TEXTURE_TYPE_AMBIENT,
//    GBUFFER_TEXTURE_TYPE_TEXCOORD,
    GBUFFER_NUM_TEXTURES
};

struct GeometryPass : public Pass {
    GLuint textures[GBUFFER_NUM_TEXTURES];
    GLuint depth_texture;
    
    GLuint fbo;
    GLuint program_id;
    GLuint vertex_array_id, vertexbuffer, normalbuffer;
    GLint mvp_id, m_id, v_id;
    GLint diffuse_color_id, ambient_color_id;
public:
    GeometryPass(int width, int height);
    
    void load_scene(const std::string &scene_path);
    
    void pass();
    
    GLuint outputTexture() const override;
    
    virtual ~GeometryPass();
    
private:
    void init_gbuffer();
    std::shared_ptr<Scene> scene;
};


#endif //DEFERRED_GEOMETRYPASS_H
