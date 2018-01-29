#ifndef OBJECT_SCENE_H
#define OBJECT_SCENE_H

#include <vector>
#include <memory>
#include <climits>

#include "model.h"

struct Scene {
    std::vector<std::shared_ptr<Model>> models;
    
    explicit Scene() = default;
    
    explicit Scene(const std::string &path, int rotated_model_vertices=INT_MAX) {
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
            if (mesh->mNumVertices > rotated_model_vertices) {
                model_ptr = std::make_shared<RotatedModel>(mesh, material);
            } else {
                model_ptr = std::make_shared<Model>(mesh, material);
            }
            if (model_ptr)
                models.emplace_back(model_ptr);
        }
    }
};

#endif //OBJECT_SCENE_H
