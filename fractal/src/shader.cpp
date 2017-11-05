// Borrowed from https://github.com/opengl-tutorials/ogl referenced by http://www.opengl-tutorial.org
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>

#include <GL/glew.h>

#include "shader.hpp"

std::string get_code(const char *path) {
    std::string code;
    std::ifstream stream(path, std::ios::in);
    if(stream.is_open()) {
        std::string line = "";
        while(getline(stream, line)) {
            code += "\n" + line;
        }
        stream.close();
    } else {
        throw std::logic_error("Impossible to open " + std::string(path));
    }
    return code;
}

void compile_shader(const std::string &shader_code, const GLuint shader_id, const std::string &shader_path = "") {
    const char *src_ptr = shader_code.c_str();
    GLint result = GL_FALSE;
    int info_log_length;

    if (shader_path.length() > 0) {
        std::cout << "Compiling shader : " << shader_path << std::endl;
    }
    glShaderSource(shader_id, 1, &src_ptr, nullptr);
    glCompileShader(shader_id);

    // Check Vertex Shader
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if ( info_log_length > 0 ) {
        std::vector<char> error_message(info_log_length + 1);
        glGetShaderInfoLog(shader_id, info_log_length, nullptr, &error_message[0]);
        const char *msg = &error_message[0];
        throw std::logic_error(msg);
    }
    assert(Result == GL_TRUE);
}

GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path) {
    // Create the shaders
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_code = get_code(vertex_file_path);
    std::string fragment_shader_code = get_code(fragment_file_path);
    GLint result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    compile_shader(vertex_shader_code, vertex_shader_id, vertex_file_path);

    // Compile Fragment Shader
    compile_shader(fragment_shader_code, fragment_shader_id, fragment_file_path);

    // Link the program
    std::cout << "Linking program\n" << std::endl;
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if ( info_log_length > 0 ) {
        std::vector<char> program_error_message(info_log_length+1);
        glGetProgramInfoLog(program_id, info_log_length, nullptr, &program_error_message[0]);
        std::cout << &program_error_message[0] << std::endl;
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}


