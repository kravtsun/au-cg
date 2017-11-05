//
// Created by kravtsun on 05.11.17.
//

#ifndef FRACTAL_TEXTURE_LOADER_H
#define FRACTAL_TEXTURE_LOADER_H

#include <GL/glew.h>

class AbstractTextureLoader {
public:
    virtual GLuint getTexture() = 0;
};


class GrayscaleTextureLoader : public AbstractTextureLoader {
public:

    GrayscaleTextureLoader(GLuint count);

    GLuint getTexture() override;

private:
	GLuint  count_;
};

#endif //FRACTAL_TEXTURE_LOADER_H
