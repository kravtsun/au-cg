// Borrowed from https://github.com/opengl-tutorials/ogl referenced by http://www.opengl-tutorial.org
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
using namespace std;

#include <GL/glew.h>

#include "shader.hpp"

std::string getCode(const char *path) {
    std::string code;
    std::ifstream stream(path, std::ios::in);
    if(stream.is_open()) {
        std::string Line = "";
        while(std::getline(stream, Line)) {
            code += "\n" + Line;
        }
        stream.close();
    } else {
        throw std::logic_error("Impossible to open " + std::string(path));
    }
    return code;
}

void compileShader(const std::string &shaderCode, GLuint shaderID, const std::string &shaderPath = "") {
    const char *srcPtr = shaderCode.c_str();
    GLint Result = GL_FALSE;
    int infoLogLength;

    if (shaderPath.length() > 0) {
        std::cout << "Compiling shader : " << shaderPath << std::endl;
    }
    glShaderSource(shaderID, 1, &srcPtr, NULL);
    glCompileShader(shaderID);

    // Check Vertex Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 ) {
        std::vector<char> errorMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errorMessage[0]);
        const char *msg = &errorMessage[0];
        throw std::logic_error(msg);
    }
    assert(Result == GL_TRUE);
}

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path) {
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string VertexShaderCode = getCode(vertex_file_path);
    std::string FragmentShaderCode = getCode(fragment_file_path);
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    compileShader(VertexShaderCode, VertexShaderID, vertex_file_path);

    // Compile Fragment Shader
    compileShader(FragmentShaderCode, FragmentShaderID, fragment_file_path);

    // Link the program
    std::cout << "Linking program\n" << endl;
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ) {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cout << &ProgramErrorMessage[0] << std::endl;
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}


