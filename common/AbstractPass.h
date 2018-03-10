#ifndef SALUTE_PASSCOMMON_H
#define SALUTE_PASSCOMMON_H

#include <string>

#include "wrappers/AllWrappersFwd.h"

typedef unsigned char uchar;

struct AbstractPass {
    AbstractPass(int width, int height);
    
    int get_width() const { return width; }
    
    int get_height() const { return height; }
    
    virtual void pass() = 0;
    
    virtual TextureWrapper output_texture() const = 0;
    
    virtual ~AbstractPass() noexcept;
private:
    const int width, height;
};

#define PASS_UNIFORM_3F(id, value)                     \
    do {                                               \
        auto value_ = (value);                         \
        glUniform3f(id, value_.x, value_.y, value_.z); \
    } while (false);

#define PASS_UNIFORM_4F(id, value)      \
    do {                                \
        auto value_ = (value);          \
        glUniform4fv(id, 1, &value_[0]); \
    } while (false);


#define PASS_UNIFORM_MAT4(id, value)                                \
    do {                                                            \
        const glm::mat4 stable_value = (value);                     \
        glUniformMatrix4fv(id, 1, GL_FALSE, &(stable_value)[0][0]); \
    } while (false);

#endif //SALUTE_PASSCOMMON_H
