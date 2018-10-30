#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "buffer.hpp"
#include "context.hpp"
#include "error.hpp"
#include "texture.hpp"

namespace opengl {

class mesh final {

public:
  struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coord;
  };

public:
  mesh(std::vector<vertex> vertices_, std::vector<size_t> indices_,
       std::vector<opengl::texture> textures_)
      : vertices(std::move(vertices_)), indices(std::move(indices_)),
        textures(std::move(textures_)) {

    if (!VBO.write(
            gsl::span(reinterpret_cast<const std::byte *>(vertices.data()),
                      vertices.size() * sizeof(vertex)))) {
      std::cerr << "VBO write failed" << std::endl;
      throw std::runtime_error("VBO write failed");
    }

    if (!VBO.vertex_attribute_pointer(0, 3, sizeof(vertex),
                                      offsetof(vertex, position))) {
      std::cerr << "VBO vertex_attribute_pointer failed" << std::endl;
      throw std::runtime_error("VBO vertex_attribute_pointer failed");
    }
    if (!VBO.vertex_attribute_pointer(1, 3, sizeof(vertex),
                                      offsetof(vertex, normal))) {
      std::cerr << "VBO vertex_attribute_pointer failed" << std::endl;
      throw std::runtime_error("VBO vertex_attribute_pointer failed");
    }
    if (!VBO.vertex_attribute_pointer(2, 3, sizeof(vertex),
                                      offsetof(vertex, texture_coord))) {
      std::cerr << "VBO vertex_attribute_pointer failed" << std::endl;
      throw std::runtime_error("VBO vertex_attribute_pointer failed");
    }
  }

  mesh(const mesh &) = delete;
  mesh &operator=(const mesh &) = delete;

  mesh(mesh &&) noexcept = delete;
  mesh &operator=(mesh &&) noexcept = delete;

  ~mesh() noexcept = default;

  bool draw() { return true; }

private:
  std::vector<vertex> vertices;
  std::vector<size_t> indices;
  std::vector<opengl::texture> textures;
  opengl::vertex_array VAO{true};
  opengl::buffer<GL_ARRAY_BUFFER, float> VBO;
};

} // namespace opengl
