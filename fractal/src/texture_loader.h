//
// Created by kravtsun on 05.11.17.
//

#ifndef FRACTAL_TEXTURE_LOADER_H
#define FRACTAL_TEXTURE_LOADER_H

#include <GL/glew.h>

class AbstractTextureLoader {
public:
    virtual GLuint get_texture() = 0;

	virtual ~AbstractTextureLoader() {}
};


class GrayscaleTextureLoader : public AbstractTextureLoader {
public:
    explicit GrayscaleTextureLoader(GLuint count);

    GLuint get_texture() override;

	virtual ~GrayscaleTextureLoader() {}

private:
	GLuint  count_;
};

#endif //FRACTAL_TEXTURE_LOADER_H
