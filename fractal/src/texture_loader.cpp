#include "texture_loader.h"

GrayscaleTextureLoader::GrayscaleTextureLoader(const GLuint count)
    : count_(count)
{}

GLuint GrayscaleTextureLoader::get_texture() {
	const auto width = count_ + 1;
	const auto data = new GLfloat[width];
    for (size_t i = 0; i < width; ++i) {
        data[i] = static_cast<GLfloat>(i) / count_;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_1D, texture_id);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, width, 0, GL_RED, GL_FLOAT, data);

    // Poor filtering:
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

//    // ... nice trilinear filtering ...
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    // ... which requires mipmaps. Generate them automatically.
//    glGenerateMipmap(GL_TEXTURE_1D);

    delete []data;

    return texture_id;
}
