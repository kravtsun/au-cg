#include <bits/stdc++.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <shader.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glfw_window_manager.h"
#include "controls.hpp"
#include "texture.hpp"

bool loadAssImp(
        const char * path,
        std::vector<unsigned short> & indices,
        std::vector<glm::vec3> & vertices,
        std::vector<glm::vec2> & uvs,
        std::vector<glm::vec3> & normals
)
{
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
    if( !scene) {
        throw std::logic_error(importer.GetErrorString());
    }
    const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)
    
    // Fill vertices positions
    vertices.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D pos = mesh->mVertices[i];
        vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
    }
    
    // Fill vertices texture coordinates
    uvs.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        uvs.push_back(glm::vec2(UVW.x, UVW.y));
    }
    
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
    
    // The "scene" pointer will be deleted automatically by "importer"
    return true;
}

bool loadOBJ(
        const char * path,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec2> & out_uvs,
        std::vector<glm::vec3> & out_normals
){
    printf("Loading OBJ file %s...\n", path);
    
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    
    
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }
    
    while( 1 ){
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                fclose(file);
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
        
    }
    
    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
        
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        
        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];
        
        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);
        
    }
    fclose(file);
    return true;
}


//struct Model {
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals; // Won't be used at the moment.
    
//    explicit Model(const std::string &filename) {
    void loadObj(const std::string &filename) {
//        bool res = loadAssImp(filename.c_str(), indices, vertices, uvs, normals);
        bool res = loadOBJ(filename.c_str(), vertices, uvs, normals);
        if (!res) {
            throw std::logic_error("Failed to load image from file: " + filename);
        }
    }
    
    void step() {
    }
//};

GLuint program_id;
GLint matrix_id, view_matrix_id, model_matrix_id;

GLuint vertex_array_id;
GLuint vertexbuffer;
std::shared_ptr<GLFWWindowManager> window_manager;
GLuint texture;
GLint texture_id;
GLuint uvbuffer;
GLuint normalbuffer;

GLint LightID;

static void init() {
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
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
    
    // Load the texture
    texture = loadDDS("uvmap.DDS");
    
    // Get a handle for our "myTextureSampler" uniform
    texture_id = glGetUniformLocation(program_id, "myTextureSampler");
    
    loadObj("suzanne.obj");
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
    glUseProgram(program_id);
    LightID = glGetUniformLocation(program_id, "LightPosition_worldspace");
}

static void paint() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program_id);
    
    // Compute the mvp matrix from keyboard and mouse input
    computeMatricesFromInputs(window_manager->window());
    glm::mat4 projection_matrix = getProjectionMatrix();
    glm::mat4 view_matrix = getViewMatrix();
    auto eye = glm::mat4(1.0);
//    glm::mat4 model_matrix = glm::rotate(eye, static_cast<float>(M_PI / 4), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 model_matrix = eye;
//    model_matrix = glm::rotate(model_matrix, static_cast<float>(M_PI / 4), glm::vec3(0.0, 1.0, 0.0));
//    model_matrix = glm::rotate(model_matrix, static_cast<float>(M_PI / 4), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;
    
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model_matrix[0][0]);
    glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view_matrix[0][0]);
    
    glm::vec3 lightPos = glm::vec3(4,4,4);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(texture_id, 0);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0, // attribute. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            static_cast<void*>(nullptr) // array buffer offset
    );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );
    
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

static void deinit() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(program_id);
    glDeleteTextures(1, &texture);
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
