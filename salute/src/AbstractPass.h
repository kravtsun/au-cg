#ifndef SALUTE_PASSCOMMON_H
#define SALUTE_PASSCOMMON_H

#include <memory>

struct Program;
using ProgramWrapper = std::shared_ptr<Program>;

struct Framebuffer;
using FramebufferWrapper = std::shared_ptr<Framebuffer>;

struct Texture;
using TextureWrapper = std::shared_ptr<Texture>;

struct VertexArray;
using VertexArrayWrapper = std::shared_ptr<VertexArray>;

struct VertexBuffer;
using VertexBufferWrapper = std::shared_ptr<VertexBuffer>;

struct AbstractPass {
    AbstractPass(int width, int height);
    
    int get_width() const { return width; }
    
    int get_height() const { return height; }
    
    virtual void pass() = 0;
    
    virtual TextureWrapper output_texture() const = 0;
    
    virtual ~AbstractPass() noexcept;

protected:
    // helpers for inheritors.
    static void init_and_bind_empty_texture(TextureWrapper &texture, int width, int height);
    
    void init_framebuffer_with_output_texture(FramebufferWrapper &fbo, TextureWrapper &color_texture) const;
    
private:
    const int width, height;
};

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

#define PASS_UNIFORM_MAT4(id, value)                                \
    do {                                                            \
        const glm::mat4 stable_value = (value);                     \
        glUniformMatrix4fv(id, 1, GL_FALSE, &(stable_value)[0][0]); \
    } while (false);

#endif //SALUTE_PASSCOMMON_H
