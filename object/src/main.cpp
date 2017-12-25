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
GLint camera_position_id;
GLint ShadowMapID;
GLint DepthBiasID;

GLuint vertex_array_id;
GLuint vertexbuffer;
std::shared_ptr<GLFWWindowManager> window_manager;
GLuint normalbuffer;

bool debug_shadowmap = false;

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
        
        ambient_color = 0.1f * diffuse_color;
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
    
        // Compute the MVP matrix from the light's point of view
        depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
        depthViewMatrix = glm::lookAt(getPosition(), glm::vec3(0,0,0), glm::vec3(0,1,0));
        
        // or, for spot light :
        //glm::vec3 lightPos(5, 20, 20);
//        depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
//        depthViewMatrix = glm::lookAt(getPosition(), getPosition() - getInvDir(), glm::vec3(0, 1, 0));
    }
    
    void step() {
    }
    
    glm::vec3 getPosition() const {
        return glm::vec3(4, 4, 4);
    }
    
    glm::vec3 getColor() const {
        return glm::vec3(1.0, 1.0, 1.0);
    }
    
    glm::vec3 getInvDir() const {
        return getPosition();
    }
    
    // TODO make static.
    GLint position_id, color_id;
    const GLuint program_id;
    
    glm::mat4 depthProjectionMatrix;
    glm::mat4 depthViewMatrix;
};

std::vector<Light> lights;
GLuint depthTexture;
GLuint depthProgramID;
GLint depthMatrixID;


GLuint FramebufferName = 0;

// debuggging:
GLuint quad_programID;
GLint texID;
GLuint quad_vertexbuffer;

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
    
    camera_position_id = glGetUniformLocation(program_id, "cameraPos");
    
    ShadowMapID = glGetUniformLocation(program_id, "shadowMap");
    DepthBiasID = glGetUniformLocation(program_id, "DepthBiasMVP");
    
    scene = std::make_shared<Scene>("scene.obj");
    
    lights.emplace_back(program_id);
    
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::logic_error("framebuffer is not ok.");
    }

    depthProgramID = load_shaders("DepthRTT.vert", "DepthRTT.frag");

    // Get a handle for our "MVP" uniform
    depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
    
    if (debug_shadowmap) {
        // debuggging:
        // The quad's FBO. Used only for visualizing the shadowmap.
        static const GLfloat g_quad_vertex_buffer_data[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
        };
    
        glGenBuffers(1, &quad_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
        quad_programID = load_shaders("Passthrough.vertexshader", "SimpleTexture.fragmentshader");
        texID = glGetUniformLocation(quad_programID, "texture_");
    }
}


#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);


static void renderToFrameBuffer(const Light &light) {
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    // We don't use bias in the shader, but instead we draw back faces,
    // which are already separated from the front faces by a small distance
    // (if your geometry is made this way)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(depthProgramID);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,  // The attribute we want to configure
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );
    
    for (auto const &model : scene->models) {
        glm::mat4 model_matrix = model->model_matrix;
        glm::mat4 depthMVP = light.depthProjectionMatrix * light.depthViewMatrix * model_matrix;
        
        glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
        glDrawArrays(GL_TRIANGLES, static_cast<GLint>(model->all_vertices_offset),
                     static_cast<GLsizei>(model->size()));
    }
    
    glDisableVertexAttribArray(0);
    
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0, 1024, 768); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
}

static void paint() {
    renderToFrameBuffer(lights.front());
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    // Compute the mvp matrix from keyboard and mouse input
    computeMatricesFromInputs(window_manager->window());
    const glm::mat4 projection_matrix = getProjectionMatrix();
    const glm::mat4 view_matrix = getViewMatrix();
    PASS_UNIFORM_3F(camera_position_id, getCameraPosition());
    
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
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(ShadowMapID, 0);
    
    const glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
    );
    
    PASS_UNIFORM_3F(light.position_id, light.getPosition());
    PASS_UNIFORM_3F(light.color_id, light.getColor());
    
    for (auto const &model : scene->models) {
        glm::mat4 model_matrix = model->model_matrix;
        glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
        glm::mat4 depthMVP = light.depthProjectionMatrix * light.depthViewMatrix * model_matrix;
        glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
        
        glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
    
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
    
//    if (debug_shadowmap) {
//        // Optionally render the shadowmap (for debug only)
//        // Render only on a corner of the window (or we we won't see the real rendering...)
//        glViewport(0, 0, 512, 512);
//
//        // Use our shader
//        glUseProgram(quad_programID);
//
//        // Bind our texture in Texture Unit 0
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, depthTexture);
//        // Set our "renderedTexture" sampler to use Texture Unit 0
//        glUniform1i(texID, 0);
//
//        // 1rst attribute buffer : vertices
//        glEnableVertexAttribArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//        glVertexAttribPointer(
//                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//                3,                  // size
//                GL_FLOAT,           // type
//                GL_FALSE,           // normalized?
//                0,                  // stride
//                (void *) 0            // array buffer offset
//        );
//
//        // Draw the triangle !
//        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
//        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//        glDisableVertexAttribArray(0);
//    }
}

static void deinit() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &vertex_array_id);
    
    glDeleteProgram(depthProgramID);
//    glDeleteTextures(1, &Texture);
    
    glDeleteFramebuffers(1, &FramebufferName);
    glDeleteTextures(1, &depthTexture);

    if (debug_shadowmap) {
        glDeleteProgram(quad_programID);
        glDeleteBuffers(1, &quad_vertexbuffer);
    }
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
