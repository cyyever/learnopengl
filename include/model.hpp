#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

#include "mesh.hpp"

namespace opengl {

class model final {

public:
  explicit model(std::filesystem::path model_file_) : model_file(model_file_) {
    if (!load(model_file)) {
      throw_exception(std::string("load model failed:") + model_file.string());
    }
  }

  model(const model &) = delete;
  model &operator=(const model &) = delete;

  model(model &&) noexcept = delete;
  model &operator=(model &&) noexcept = delete;

  ~model() noexcept = default;

  bool draw(opengl::program &prog) {
    auto draw_mesh =
        [this,
         &prog](auto &&self,
                const std::unique_ptr<tree_node<opengl::mesh>> &node) -> bool {
      if (!node->value.draw(prog)) {
        return false;
      }
      for (const auto &child : node->children) {
        if (!self(self, child)) {
          return false;
        }
      }
      return true;
    };

    return draw_mesh(draw_mesh, meshes);
  }

private:
  bool load(std::filesystem::path model_file) {
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(
        model_file.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return false;
    }

    auto process_node =
        [&scene,
         this](auto &&self, auto assimp_node,
               std::unique_ptr<tree_node<opengl::mesh>> &new_node) -> void {
      for (size_t i = 0; i < assimp_node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[assimp_node->mMeshes[i]];
        new_node->value = convert_assimp_mesh(*mesh, *scene);
      }
      // then do the same for each of its children
      for (size_t i = 0; i < assimp_node->mNumChildren; i++) {
        std::unique_ptr<tree_node<opengl::mesh>> child;
        self(self, assimp_node->mChildren[i], child);
        new_node->children.emplace_back(std::move(child));
      }
    };

    process_node(process_node, scene->mRootNode, meshes);
    return true;
  }

  ::opengl::mesh convert_assimp_mesh(const ::aiMesh &assimp_mesh,
                                     const ::aiScene &assimp_scene) {

    std::vector<mesh::vertex> vertices;
    for (unsigned int i = 0; i < assimp_mesh.mNumVertices; i++) {
      mesh::vertex vertex;
      vertex.position.x = assimp_mesh.mVertices[i].x;
      vertex.position.y = assimp_mesh.mVertices[i].y;
      vertex.position.z = assimp_mesh.mVertices[i].z;

      vertex.normal.x = assimp_mesh.mNormals[i].x;
      vertex.normal.y = assimp_mesh.mNormals[i].y;
      vertex.normal.z = assimp_mesh.mNormals[i].z;

      if (assimp_mesh
              .mTextureCoords[0]) // does the mesh contain texture coordinates?
      {
        vertex.texture_coord.x = assimp_mesh.mTextureCoords[0][i].x;
        vertex.texture_coord.y = assimp_mesh.mTextureCoords[0][i].y;
      } else {
        vertex.texture_coord = {};
      }
      vertices.emplace_back(std::move(vertex));
    }

    std::vector<GLuint> indices;
    for (unsigned int i = 0; i < assimp_mesh.mNumFaces; i++) {
      auto const &face = assimp_mesh.mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }

    std::vector<opengl::texture> diffuse_textures;
    std::vector<opengl::texture> specular_textures;
    if (assimp_mesh.mMaterialIndex >= 0) {
      auto const material = assimp_scene.mMaterials[assimp_mesh.mMaterialIndex];
      diffuse_textures = load_assimp_texture(*material, aiTextureType_DIFFUSE);
      specular_textures =
          load_assimp_texture(*material, aiTextureType_SPECULAR);
    }

    return ::opengl::mesh(vertices, indices, {});
  }

  std::vector<opengl::texture> load_assimp_texture(const ::aiMaterial &material,
                                                   ::aiTextureType type) {
    std::vector<opengl::texture> textures;
    for (size_t i = 0; i < material.GetTextureCount(type); i++) {
      aiString file_path;
      material.GetTexture(type, i, &file_path);
      auto abs_path = std::filesystem::absolute(model_file.parent_path() /
                                                file_path.C_Str());
      auto [it, has_emplaced] = loaded_textures.try_emplace(
          abs_path, GL_TEXTURE_2D, next_texture_unit, abs_path);
      if (has_emplaced) {
        next_texture_unit++;
      }
      textures.push_back(it->second);
    }
    return textures;
  }

private:
  std::filesystem::path model_file;
  template <typename T> struct tree_node {
    T value;
    std::vector<std::unique_ptr<tree_node>> children;
  };
  std::unique_ptr<tree_node<opengl::mesh>> meshes;

  std::map<std::filesystem::path, opengl::texture> loaded_textures;
  GLenum next_texture_unit{GL_TEXTURE0};
};

} // namespace opengl
