#ifndef SALUTE_PASSTHROUGHPASS_H
#define SALUTE_PASSTHROUGHPASS_H

#include <string>
#include <GL/glew.h>
#include "AbstractPass.h"

struct PassthroughPass: AbstractPass {
    /**
     * @param width
     * @param height
     * @param direct is PassthroughPass used for rendering to the default framebuffer
     * or as just a container for rendering quad?
     * Have to be set to direct=false when deriving from PassthroughPass.
     */
    PassthroughPass(int width, int height, bool direct=true);
    
    void pass() override;
    
    void set_input_texture(TextureWrapper new_input_texture);
    
    TextureWrapper output_texture() const override;
    
    virtual ~PassthroughPass();

protected:
    void draw_quad() const;
    
    TextureWrapper get_input_texture() const;
    
    void check_input_texture_set(const std::string &caller) const;

private:
    bool is_direct;
    ProgramWrapper program;
    GLint pass_texture_id;
    TextureWrapper input_texture;
    
    GLuint quad_vao;
    GLuint quad_vertexbuffer;
};


#endif //SALUTE_PASSTHROUGHPASS_H
