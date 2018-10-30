#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "buffer.hpp"
#include "error.hpp"
#include "program.hpp"
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
  mesh(std::vector<vertex> vertices_, std::vector<GLuint> indices_,
       std::vector<std::pair<opengl::texture, std::string>> textures_)
      : vertices(std::move(vertices_)), indices(std::move(indices_)),
        textures(std::move(textures_)) {

    if (!VEO.write(indices)) {
      std::cerr << "VEO write failed" << std::endl;
      throw std::runtime_error("VEO write failed");
    }

    if (!VBO.write(vertices)) {
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

  mesh(mesh &&) noexcept = default;
  mesh &operator=(mesh &&) noexcept = default;

  ~mesh() noexcept = default;

  bool draw(opengl::program &prog) {
    if (!prog.use()) {
      return false;
    }
    if (!VAO.use()) {
      return false;
    }

    for (auto &[texture, variable_name] : textures) {
      if (!prog.set_uniform(variable_name, texture)) {
        return -1;
      }
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    if (check_error()) {
      std::cerr << "glDrawElements failed" << std::endl;
      return false;
    }

    return true;
  }

private:
  std::vector<vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<std::pair<opengl::texture, std::string>> textures;
  opengl::vertex_array VAO{true};
  opengl::buffer<GL_ARRAY_BUFFER, float> VBO;
  opengl::buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint> VEO;
};

} // namespace opengl
