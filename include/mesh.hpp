#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
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
       std::map<texture::type, std::vector<opengl::texture>> textures_)
      : vertices(std::move(vertices_)), indices(std::move(indices_)),
        textures(std::move(textures_)) {

    if (!VAO.use()) {
      throw_exception("use VAO failed");
    }
    if (!EBO.write(indices)) {
      throw_exception("EBO write failed");
    }

    if (!VBO.write(vertices)) {
      throw_exception("VBO write failed");
    }

    if (!VBO.vertex_attribute_pointer(0, 3, sizeof(vertex),
                                      offsetof(vertex, position))) {
      throw_exception("VBO vertex_attribute_pointer failed");
    }
    if (!VBO.vertex_attribute_pointer(1, 3, sizeof(vertex),
                                      offsetof(vertex, normal))) {
      throw_exception("VBO vertex_attribute_pointer failed");
    }

    if (!VBO.vertex_attribute_pointer(2, 2, sizeof(vertex),
                                      offsetof(vertex, texture_coord))) {
      throw_exception("VBO vertex_attribute_pointer failed");
    }

    if (!VAO.unuse()) {
      throw_exception("unuse VAO failed");
    }
  }

  mesh(const mesh &) = delete;
  mesh &operator=(const mesh &) = delete;

  mesh(mesh &&) = default;
  mesh &operator=(mesh &&) = default;

  ~mesh() noexcept = default;

  bool draw(opengl::program &prog,
            const std::map<texture::type, std::vector<std::string>>
                &texture_variable_names) {
    prog.set_vertex_array(VAO);
    for (auto const &[type, variable_names] : texture_variable_names) {
      auto it = textures.find(type);
      if (it == textures.end()) {
        std::cerr << "no texture for type " << static_cast<int>(type)
                  << std::endl;
        return false;
      }
      if (variable_names.size() > it->second.size()) {
        std::cerr << "more variable then texture:" << variable_names.size()
                  << ' ' << it->second.size() << std::endl;
        return false;
      }
      if (variable_names.size() < it->second.size()) {
        std::cerr << "less variable then texture:" << variable_names.size()
                  << ' ' << it->second.size() << std::endl;
      }
      for (size_t i = 0; i < variable_names.size(); i++) {
        if (!prog.set_uniform(variable_names[i], it->second[i])) {
          return false;
        }
      }
    }

    if (!prog.use()) {
      return false;
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
  std::map<texture::type, std::vector<opengl::texture>> textures;
  opengl::vertex_array VAO{true};
  opengl::buffer<GL_ARRAY_BUFFER, float> VBO;
  opengl::buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint> EBO;
};

} // namespace opengl
