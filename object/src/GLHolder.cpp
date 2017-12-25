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
GLint ShadowMapID;
GLint DepthBiasID;

GLuint vertex_array_id;
GLuint vertexbuffer;
GLuint normalbuffer;

bool debug_shadowmap = false;

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
GLuint depthTexture;
GLuint depthProgramID;
GLint depthMatrixID;

GLuint FramebufferName = 0;

// debuggging:
GLuint quad_programID;
GLint texID;
GLuint quad_vertexbuffer;

GLHolder::GLHolder(std::shared_ptr<GLFWWindowManager> window_manager)
        : window_manager(window_manager)
{
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    if (!debug_shadowmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    }
    
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

GLHolder::~GLHolder() {
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
            nullptr             // array buffer offset
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

#define PASS_UNIFORM_3F(id, value) \
    glUniform3f(id, (value).x, (value).y, (value).z);

void GLHolder::paint() {
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
    
    auto &light = lights.front();
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
    light.step();
    
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
    
    if (debug_shadowmap) {
        // Optionally render the shadowmap (for debug only)
        // Render only on a corner of the window (or we we won't see the real rendering...)
        glViewport(0, 0, 512, 512);
        
        // Use our shader
        glUseProgram(quad_programID);
        
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        // Set our "renderedTexture" sampler to use Texture Unit 0
        glUniform1i(texID, 0);
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr             // array buffer offset
        );
        
        // Draw the triangle !
        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
        glDisableVertexAttribArray(0);
    }
}
