#ifndef FRACTAL_TEXTURE_LOADER_H
#define FRACTAL_TEXTURE_LOADER_H

#include <qopengl.h>

class AbstractTextureLoader {
public:
    virtual GLuint getTexture() = 0;
};

class GrayscaleTextureLoader : public AbstractTextureLoader {
public:

    GrayscaleTextureLoader(size_t count);

    GLuint getTexture() override;

private:
    size_t  count_;
};




#endif //FRACTAL_TEXTURE_LOADER_H
