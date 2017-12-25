#include <memory>

#include "GLHolder.h"
#include "shader.hpp"
#include "model.h"
#include "light.h"
#include "controls.hpp"

GLuint program_id;
GLint matrix_id, view_matrix_id, model_matrix_id;
GLint diffuse_color_id, ambient_color_id, specular_color_id;
GLint camera_position_id;

GLuint vertex_array_id;
GLuint vertexbuffer;
GLuint normalbuffer;

struct Scene {
    std::vector<std::shared_ptr<Model>> models;
    
    explicit Scene() = default;
    
    explicit Scene(const std::string &path) {
        Assimp::Importer importer;
        
        const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
        if( !scene) {
            throw std::logic_error(importer.GetErrorString());
        }
        
        for (int imesh = 0; imesh < scene->mNumMeshes; ++imesh) {
            const aiMesh* mesh = scene->mMeshes[imesh];
            const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::shared_ptr<Model> model_ptr;
//            if (mesh->mName == aiString("suzanne")) {
//            if (strstr("bunny", mesh->mName.C_Str()) != NULL) {
            if (mesh->mNumVertices > 30000) {
                model_ptr = std::make_shared<RotatedModel>(mesh, material);
            } else {
                model_ptr = std::make_shared<Model>(mesh, material);
            }
            if (model_ptr)
                models.emplace_back(model_ptr);
        }
        
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, Model::all_vertices.size() * sizeof(glm::vec3), &Model::all_vertices[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, Model::all_normals.size() * sizeof(glm::vec3), &Model::all_normals[0], GL_STATIC_DRAW);
    }
    
};

std::shared_ptr<Scene> scene;

// TODO RotationModel.

std::vector<Light> lights;
GLuint depthProgramID;

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
{
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glUseProgram(program_id);
    
    program_id = load_shaders("vertex_shader.vert", "fragment_shader.frag");
    
    matrix_id = glGetUniformLocation(program_id, "MVP");
    view_matrix_id = glGetUniformLocation(program_id, "V");
    model_matrix_id = glGetUniformLocation(program_id, "M");
    
    diffuse_color_id = glGetUniformLocation(program_id, "diffuse_color");
    ambient_color_id = glGetUniformLocation(program_id, "ambient_color");
    specular_color_id = glGetUniformLocation(program_id, "specular_color");
    
    camera_position_id = glGetUniformLocation(program_id, "cameraPos");
    
    scene = std::make_shared<Scene>("scene.obj");

    depthProgramID = load_shaders("DepthRTT.vert", "DepthRTT.frag");
    lights.emplace_back(program_id, "shadowMap", "DepthBiasVP", "lightPos", "lightColor", depthProgramID, "depthMVP");
}

GLHolder::~GLHolder() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    
    glDeleteProgram(depthProgramID);
//    glDeleteTextures(1, &Texture);
}

void GLHolder::paint() {
    // FIXME.
    auto &light = lights.front();
//    for (auto &light : lights) {
    light.step();
    light.paint(vertexbuffer, scene->models);
//    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program_id);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            static_cast<void *>(nullptr) // array buffer offset
    );
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            static_cast<void *>(nullptr)      // array buffer offset
    );
    
    if (window_manager) {
        computeMatricesFromInputs(window_manager->window());
    }
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
    PASS_UNIFORM_3F(camera_position_id, getCameraPosition());
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, light.depthTexture);
    glUniform1i(light.ShadowMapID, 0);
    PASS_UNIFORM_3F(light.position_id, light.getPosition());
    PASS_UNIFORM_3F(light.color_id, light.getColor());
    auto depthBiasVP = light.getDepthBiasVP();
    glUniformMatrix4fv(light.DepthBiasID, 1, GL_FALSE, &depthBiasVP[0][0]);
    
    for (auto const &model : scene->models) {
        glm::mat4 model_matrix = model->model_matrix;
        glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
        
        PASS_UNIFORM_3F(diffuse_color_id, model->diffuse_color);
        PASS_UNIFORM_3F(ambient_color_id, model->ambient_color);
        PASS_UNIFORM_3F(specular_color_id, model->specular_color);
        
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset),
                     static_cast<GLsizei>(model->size()));
        model->step();
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
