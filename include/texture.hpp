#pragma once

#include <filesystem>
#include <gsl/gsl>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace opengl {

class texture final {
public:
  struct extra_config {
    bool generate_mipmap{true};
  };

  texture(GLenum target_, GLenum unit_, std::filesystem::path image)
      : target(target_), unit(unit_) {
    glGenTextures(1, &texture_id);
    if (glGetError() != GL_NO_ERROR) {
      std::cerr << "glGenTextures failed" << std::endl;
      throw std::runtime_error("glBindTexture failed");
    }
    std::unique_ptr<GLuint, std::function<void(GLuint *)>> clean_texture(
        &texture_id, [](auto p) { glDeleteTextures(1, p); });

    extra_config config;
    config.generate_mipmap = false;
    if (!use(config)) {
      throw std::runtime_error("use texture failed");
    }

    if (!std::filesystem::exists(image)) {
      std::cerr << "no image " << image << std::endl;
      throw std::runtime_error("no image");
    }
    stbi_set_flip_vertically_on_load(image.extension() == ".png");

    int width, height, channel;
    auto *data =
        stbi_load(image.string().c_str(), &width, &height, &channel, 0);
    if (!data) {
      std::cerr << "stbi_load " << image << " failed" << std::endl;
      throw std::runtime_error("stbi_load failed");
    }
    auto cleanup = gsl::finally([data]() { stbi_image_free(data); });
    GLenum format = 0;
    if (channel == 3) {
      format = GL_RGB;
    } else if (channel == 4) {
      format = GL_RGBA;
    } else {
      throw std::runtime_error("unsupported channels");
    }
    glTexImage2D(target, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE,
                 data);
    if (glGetError() != GL_NO_ERROR) {
      std::cerr << "glTexImage2D failed" << std::endl;
      throw std::runtime_error("glTexImage2D failed");
    }
    clean_texture.release();
  }

  texture(const texture &) = delete;
  texture &operator=(const texture &) = delete;

  texture(texture &&) noexcept = default;
  texture &operator=(texture &&) noexcept = default;

  ~texture() noexcept { glDeleteTextures(1, &texture_id); }

  bool use(const extra_config &config = {}) noexcept {
    glActiveTexture(unit);
    if (glGetError() != GL_NO_ERROR) {
      std::cerr << "glActiveTexture failed" << std::endl;
      return false;
    }
    glBindTexture(target, texture_id);
    if (glGetError() != GL_NO_ERROR) {
      std::cerr << "glBindTexture failed" << std::endl;
      return false;
    }
    if (config.generate_mipmap) {
      glGenerateMipmap(target);
      if (glGetError() != GL_NO_ERROR) {
        std::cerr << "glGenerateMipmap failed" << std::endl;
        return false;
      }
    }
    return true;
  }

  template <typename value_type>
  bool set_parameter(GLenum pname, value_type value) noexcept {
    if constexpr (std::is_same_v<value_type, GLint>) {
      glTexParameteri(target, pname, value);
    } else if constexpr (std::is_same_v<value_type, GLfloat>) {
      glTexParameterf(target, pname, value);
    } else {
      static_assert("not supported value type");
    }
    if (glGetError() != GL_NO_ERROR) {
      std::cerr << "glTexParameter failed" << std::endl;
      return false;
    }
    return true;
  }

  GLenum get_unit() const { return unit; }

private:
  GLuint texture_id{0};
  GLenum target{};
  GLenum unit{};
};

} // namespace opengl
