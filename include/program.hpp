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
    auto shader_id = glCreateShader(shader_type);
    if (shader_id == 0) {
      std::cout << "glCreateShader failed" << std::endl;
      return false;
    }
    auto source_data = source_code.data();
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

  bool use() noexcept {
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
    }
    glUseProgram(program_id);
    return true;
  }

  GLuint get_program_id() const noexcept { return program_id; }

private:
  GLuint program_id{0};
  bool linked{false};
};
} // namespace opengl
