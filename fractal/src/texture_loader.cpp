//
// Created by kravtsun on 05.11.17.
//

#include "texture_loader.h"

GrayscaleTextureLoader::GrayscaleTextureLoader(size_t count)
    : count_(count)
{}

GLuint GrayscaleTextureLoader::getTexture() {
    GLuint width = count_ + 1;
    GLfloat *data = new GLfloat[width];
    for (int i = 0; i < width; ++i) {
        data[i] = (GLfloat)i / count_;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_1D, textureID);

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

    return textureID;
}
