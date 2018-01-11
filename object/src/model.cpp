#include <iterator>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"

#define all(x) (x).begin(), (x).end()

std::vector<glm::vec3> Model::all_vertices;
std::vector<glm::vec3> Model::all_normals;

Model::Model(const aiMesh *mesh, const aiMaterial *material) {
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
    
    aiColor4D tmp_color;
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &tmp_color)) {
        assert(tmp_color.a == 1.0);
        diffuse_color = glm::vec3(tmp_color.r, tmp_color.g, tmp_color.b);
    }

    // default ambient color.
    ambient_color = 0.1f * diffuse_color;
    
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &tmp_color)) {
        assert(tmp_color.a == 1.0);
        ambient_color = glm::vec3(tmp_color.r, tmp_color.g, tmp_color.b);
    }
    
    if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &tmp_color)) {
        assert(tmp_color.a == 1.0);
        specular_color = glm::vec3(tmp_color.r, tmp_color.g, tmp_color.b);
    }
}

RotatedModel::RotatedModel(const aiMesh *mesh, const aiMaterial *material) : Model(mesh, material) {}

void RotatedModel::step() {
    Model::step();
    model_matrix = glm::rotate(model_matrix, rotation_step, glm::vec3(0.0, 1.0, 0.0));
}
