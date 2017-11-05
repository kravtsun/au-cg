#ifndef SHADER_HPP
#define SHADER_HPP

#include <QOpenGLFunctions_3_3_Core>

class ShaderLoader {
public:
    ShaderLoader(QOpenGLFunctions_3_3_Core *glfun)
        : glfun(glfun)
    {}

    GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

private:
    void compileShader(const std::string &shaderCode, GLuint shaderID, const std::string &shaderPath = "");

    QOpenGLFunctions_3_3_Core *glfun;
};

#endif
