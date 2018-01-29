#ifndef OBJECT_MODEL_H
#define OBJECT_MODEL_H

#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct Model {
    static std::vector<glm::vec3> all_vertices;
    static std::vector<glm::vec3> all_normals;
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    size_t all_vertices_offset = 0;
    size_t all_normals_offset = 0;
    
    glm::mat4x4 model_matrix = glm::mat4(1.0);
    
    glm::vec3 diffuse_color = glm::vec3(0);
    glm::vec3 ambient_color = glm::vec3(0);
    glm::vec3 specular_color = glm::vec3(0);
    
    explicit Model(const aiMesh* mesh, const aiMaterial* material);
    
    virtual void step() {}
    
    size_t size() const {
        return vertices.size();
    }
    
    virtual ~Model() = default;
};


struct RotatedModel : public Model {
    static constexpr float rotation_step = 0.05f;
    
    RotatedModel(const aiMesh *mesh, const aiMaterial* material);
    
    void step() override;
};

#endif //OBJECT_MODEL_H
