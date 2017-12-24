#include <bits/stdc++.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "glfw_window_manager.h"
#include "controls.hpp"
#include "texture.hpp"

#define all(x) (x).begin(), (x).end()

GLuint program_id;
GLint matrix_id, view_matrix_id, model_matrix_id;
GLint diffuse_color_id, ambient_color_id, specular_color_id;
GLint ka_id, kd_id, ks_id, shiness_id;
GLuint vertex_array_id;
GLuint vertexbuffer;
std::shared_ptr<GLFWWindowManager> window_manager;
GLuint normalbuffer;

struct Model {
    static std::vector<glm::vec3> all_vertices;
    static std::vector<glm::vec3> all_normals;
    static constexpr float rotation_step = 0.05f;
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    size_t all_vertices_offset = 0;
    size_t all_normals_offset = 0;
    
    glm::mat4x4 model_matrix = glm::mat4(1.0);
    
    glm::vec3 diffuse_color = glm::vec3(0);
    glm::vec3 ambient_color = glm::vec3(0);
    glm::vec3 specular_color = glm::vec3(0);
    
//    float Ka;   // Ambient reflection coefficient
//    float Kd;   // Diffuse reflection coefficient
//    float Ks;   // Specular reflection coefficient
//    float shininessVal; // Shininess

    explicit Model(const aiMesh* mesh, const aiMaterial* material) {
        vertices.reserve(mesh->mNumVertices);
        for(auto i = 0; i < mesh->mNumVertices; i++){
            aiVector3D pos = mesh->mVertices[i];
            vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
        }
        //    // Fill vertices texture coordinates
        //    uvs.reserve(mesh->mNumVertices);
        //    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        //        aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        //        uvs.push_back(glm::vec2(UVW.x, UVW.y));
        //    }
    
        // Fill vertices normals
        normals.reserve(mesh->mNumVertices);
        for(unsigned int i=0; i<mesh->mNumVertices; i++){
            aiVector3D n = mesh->mNormals[i];
            normals.push_back(glm::vec3(n.x, n.y, n.z));
        }
    
        // Fill face indices
        indices.reserve(3*mesh->mNumFaces);
        for (unsigned int i=0; i<mesh->mNumFaces; i++){
            // Assume the model has only triangles.
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[1]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
    
        all_vertices_offset = all_vertices.size();
        all_normals_offset = all_normals.size();
        std::copy(all(vertices), std::back_inserter(all_vertices));
        std::copy(all(normals), std::back_inserter(all_normals));
        
//        for (int i = 0; i < material->mNumProperties; ++i) {
            aiColor4D color;
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color)) {
                assert(color.a == 1.0);
                diffuse_color = glm::vec3(color.r, color.g, color.b);
            }
    
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color)) {
                assert(color.a == 1.0);
                ambient_color = glm::vec3(color.r, color.g, color.b);
            }
    
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color)) {
                assert(color.a == 1.0);
                specular_color = glm::vec3(color.r, color.g, color.b);
            }
            
//            if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_AMBIENT))
            // Defines the shininess of a phong-shaded material. This is actually the exponent of the phong specular equation
//        }
    }

    virtual void step() {}
    
    size_t size() const {
        return vertices.size();
    }
    
    virtual ~Model() {
    }
};

std::vector<glm::vec3> Model::all_vertices;
std::vector<glm::vec3> Model::all_normals;

struct RotatedModel : public Model {
    RotatedModel(const aiMesh *mesh, const aiMaterial* material) : Model(mesh, material) {}
    
    void step() override {
        Model::step();
        model_matrix = glm::rotate(model_matrix, rotation_step, glm::vec3(0.0, 1.0, 0.0));
    }
};

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

struct Light {
    explicit Light(GLuint program_id)
            : program_id(program_id)
    {
        glUseProgram(program_id);
        position_id = glGetUniformLocation(program_id, "lightPos");
        color_id = glGetUniformLocation(program_id, "lightColor");
    }
    
    void step() {
    }
    
    glm::vec3 getPosition() const {
        return glm::vec3(4, 4, 4);
    }
    
    glm::vec3 getColor() const {
        return glm::vec3(1.0, 1.0, 1.0);
    }
    
    // TODO make static.
    GLint position_id, color_id;
    const GLuint program_id;
};

std::vector<Light> lights;

static void init() {
    // Dark blue background
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    glUseProgram(program_id);
    
    // Create and compile our GLSL program from the shaders
    program_id = load_shaders("vertex_shader.vert", "fragment_shader.frag");

    // Get a handle for our "MVP" uniform
    matrix_id = glGetUniformLocation(program_id, "MVP");
    view_matrix_id = glGetUniformLocation(program_id, "V");
    model_matrix_id = glGetUniformLocation(program_id, "M");
    
    diffuse_color_id = glGetUniformLocation(program_id, "diffuse_color");
    ambient_color_id = glGetUniformLocation(program_id, "ambient_color");
    specular_color_id = glGetUniformLocation(program_id, "specular_color");
    
//    ka_id = glGetUniformLocation(program_id, "Ka");
//    kd_id = glGetUniformLocation(program_id, "Kd");
//    ks_id = glGetUniformLocation(program_id, "Ks");
//    shiness_id = glGetUniformLocation(program_id, "shininessVal");
    
    scene = std::make_shared<Scene>("scene.obj");
    
    lights.emplace_back(program_id);
}


static void paint() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    // Compute the mvp matrix from keyboard and mouse input
    computeMatricesFromInputs(window_manager->window());
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
    
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
    
    auto const &light = lights.front();
    auto light_position = light.getPosition();

#define PASS_UNIFORM_3F(value, id) \
    glUniform3f(id, (value).x, (value).y, (value).z);
    
    PASS_UNIFORM_3F(light.getPosition(), light.position_id);
    PASS_UNIFORM_3F(light.getColor(), light.color_id);
    
    for (auto const &model : scene->models) {
        glm::mat4 model_matrix = model->model_matrix;
        glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
    
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
        
        PASS_UNIFORM_3F(model->diffuse_color, diffuse_color_id);
        PASS_UNIFORM_3F(model->ambient_color, ambient_color_id);
        PASS_UNIFORM_3F(model->specular_color, specular_color_id);
        
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset),
                     static_cast<GLsizei>(model->size()));
        model->step();
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

static void deinit() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &vertex_array_id);
}

int main() {
    window_manager = std::make_shared<GLFWWindowManager>("Object", 1024, 768);
    init();

    window_manager->main_loop([]()
                              {
                                  paint();
                              });

    deinit();
 
    return 0;
}
