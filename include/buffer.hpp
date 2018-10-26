#pragma once

#include <iostream>
#include <stdexcept>

#include "error.hpp"

namespace opengl {

template <GLenum target, typename data_type> class buffer final {
  static_assert(std::is_same_v<GLfloat, data_type> ||
                    std::is_same_v<GLint, data_type>,
                "unsupported data type");

public:
  explicit buffer() {
    glGenBuffers(1, &buffer_id);

    if (check_error()) {
      std::cerr << "glGenBuffers failed" << std::endl;
      throw std::runtime_error("glGenBuffers failed");
    }
  }

  buffer(const buffer &) = delete;
  buffer &operator=(const buffer &) = delete;

  buffer(buffer &&) noexcept = delete;
  buffer &operator=(buffer &&) noexcept = delete;

  ~buffer() noexcept { glDeleteBuffers(1, &buffer_id); }

  template <size_t N> bool write(data_type (&const data)[N]) noexcept {
    if constexpr (N == 0) {
      std::cerr << "can't write empty array" << std::endl;
      return false;
    }

    if (!bind()) {
      return false;
    }

    glBufferData(target, sizeof(data), data, GL_STATIC_DRAW);
    if (check_error()) {
      std::cerr << "glBufferData failed" << std::endl;
      return false;
    }
    return true;
  }

  template <typename = std::enable_if_t<target == GL_ARRAY_BUFFER>>
  bool vertex_attribute_pointer(GLuint index, GLint size, GLsizei stride,
                                size_t offset) noexcept {
    if (!bind()) {
      return false;
    }

    GLenum type = GL_FLOAT;
    if constexpr (std::is_same_v<GLint, data_type>) {
      type = GL_INT;
    }
    glVertexAttribIPointer(index, size, type, stride * sizeof(data_type),
                           (void *)offset);
    if (check_error()) {
      std::cerr << "glVertexAttribIPointer failed" << std::endl;
      return false;
    }

    glEnableVertexAttribArray(index);
    if (check_error()) {
      std::cerr << "glEnableVertexAttribArray failed" << std::endl;
      return false;
    }
    return true;
  }

private:
  bool bind() noexcept {
    glBindBuffer(target, buffer_id);
    if (check_error()) {
      std::cerr << "glBindBuffer failed" << std::endl;
      return false;
    }
    return true;
  }

private:
  GLuint buffer_id{0};
};

class vertex_array final {

public:
  explicit vertex_array(bool use_after_create = true) {
    glGenVertexArrays(1, &vertex_array_id);

    if (check_error()) {
      std::cerr << "glGenVertexArrays failed" << std::endl;
      throw std::runtime_error("glGenVertexArrays failed");
    }
    if (use_after_create && !use()) {
      throw std::runtime_error("can't use vertex_array");
    }
  }

  vertex_array(const vertex_array &) = delete;
  vertex_array &operator=(const vertex_array &) = delete;

  vertex_array(vertex_array &&) noexcept = delete;
  vertex_array &operator=(vertex_array &&) noexcept = delete;

  ~vertex_array() noexcept { glDeleteVertexArrays(1, &vertex_array_id); }

  bool use() noexcept { return bind(); }

private:
  bool bind() noexcept {
    glBindVertexArray(vertex_array_id);
    if (check_error()) {
      std::cerr << "glBindVertexArray failed" << std::endl;
      return false;
    }
    return true;
  }

private:
  GLuint vertex_array_id{0};
};
} // namespace opengl
