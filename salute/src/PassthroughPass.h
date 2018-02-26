#ifndef SALUTE_PASSTHROUGHPASS_H
#define SALUTE_PASSTHROUGHPASS_H

#include "AbstractPass.h"

struct PassthroughPass: public AbstractPass {
    /**
     * @param width
     * @param height
     * @param direct is PassthroughPass used for rendering to the default framebuffer
     * or as just a container for rendering quad?
     * Have to be set to direct=false when deriving from PassthroughPass.
     */
    PassthroughPass(int width, int height, bool direct=true);
    
    void pass() override;
    
    void set_input_texture(GLuint new_input_texture) {
        input_texture = new_input_texture;
    }
    
    GLuint output_texture() const override;
    
    virtual ~PassthroughPass();

protected:
    void draw_quad() const;
    
    GLuint get_input_texture() const {
        return input_texture;
    }
    
    void check_input_texture_set(const std::string &caller="PassthroughPass") const;

private:
    bool is_direct;
    GLuint program_id;
    GLint pass_texture_id;
    GLuint input_texture = static_cast<GLuint>(-1);
    
    GLuint quad_vertexbuffer;
};


#endif //SALUTE_PASSTHROUGHPASS_H
