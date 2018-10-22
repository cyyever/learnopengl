#pragma once

#include <iostream>
#include <stdexcept>
#include <string_view>

namespace opengl {

class program final {
public:
  program() {
    program_id = glCreateProgram();
    if (program_id == 0) {
      std::cerr << "glCreateProgram failed" << std::endl;
      throw std::runtime_error("glCreateProgram failed");
    }
  }

  program(const program &) = delete;
  program &operator=(const program &) = delete;

  program(program &&) noexcept = default;
  program &operator=(program &&) noexcept = default;

  bool attach_shader(GLenum shader_type,
                     std::string_view source_code) noexcept {
    const auto shader_id = glCreateShader(shader_type);
    if (shader_id == 0) {
      std::cerr << "glCreateShader failed" << std::endl;
      return false;
    }
    const auto source_data = source_code.data();
    GLint source_size = source_code.size();

    glShaderSource(shader_id, 1, &source_data, &source_size);
    glCompileShader(shader_id);

    GLint success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
      GLchar infoLog[512]{};
      glGetShaderInfoLog(shader_id, sizeof(infoLog), nullptr, infoLog);
      std::cerr << "glCompileShader failed" << infoLog << std::endl;
      return false;
    }
    linked = false;
    glAttachShader(program_id, shader_id);
    glDeleteShader(shader_id);
    return true;
  }

  bool use() {
    if (!link()) {
      return false;
    }
    glUseProgram(program_id);
    return true;
  }

  bool
  set_uniform_by_callback(const std::string &variable_name,
                          std::function<void(GLint location)> set_function) {
    if (!link()) {
      return false;
    }
    auto location = glGetUniformLocation(program_id, variable_name.c_str());
    if (location == -1) {
      std::cerr << "glGetUniformLocation failed" << std::endl;
      return false;
    }
    set_function(location);
    return true;
  }

  template <typename value_type>
  bool set_uniform(const std::string &variable_name,
                   value_type value) noexcept {
    if constexpr (std::is_same_v<value_type, GLint>) {
      return set_uniform_by_callback(variable_name, [value](auto location) {
        glUniform1i(location, value);
      });
    } else if constexpr (std::is_same_v<value_type, GLfloat>) {
      return set_uniform_by_callback(variable_name, [value](auto location) {
        glUniform1f(location, value);
      });
    } else {
      static_assert("not supported value type");
    }
    return false;
  }

private:
  bool link() {
    if (!linked) {
      glLinkProgram(program_id);

      GLint success = 0;
      glGetProgramiv(program_id, GL_LINK_STATUS, &success);
      if (!success) {
        GLchar infoLog[512]{};
        glGetProgramInfoLog(program_id, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "glLinkProgram failed" << infoLog << std::endl;
        return false;
      }
      linked = true;
    }
    return true;
  }

private:
  GLuint program_id{0};
  bool linked{false};
};
} // namespace opengl
