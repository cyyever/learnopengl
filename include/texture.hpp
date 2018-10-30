#pragma once

#include <filesystem>
#include <gsl/gsl>
#include <iostream>
#include <memory>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "error.hpp"

namespace opengl {

class texture final {
public:
  struct extra_config {
    extra_config() : generate_mipmap{true} {};
    bool generate_mipmap;
  };

  texture(GLenum target_, GLenum unit_, std::filesystem::path image)
      : target(target_), unit(unit_) {
    glGenTextures(1, &texture_id);
    if (check_error()) {
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
    if (check_error()) {
      std::cerr << "glTexImage2D failed" << std::endl;
      throw std::runtime_error("glTexImage2D failed");
    }
    clean_texture.release();
  }

  texture(const texture &) = default;
  texture &operator=(const texture &) = default;

  texture(texture &&) noexcept = default;
  texture &operator=(texture &&) noexcept = default;

  ~texture() noexcept { glDeleteTextures(1, &texture_id); }

  bool use(const extra_config &config = {}) noexcept {
    glActiveTexture(unit);
    if (check_error()) {
      std::cerr << "glActiveTexture failed" << std::endl;
      return false;
    }
    glBindTexture(target, texture_id);
    if (check_error()) {
      std::cerr << "glBindTexture failed" << std::endl;
      return false;
    }
    if (config.generate_mipmap) {
      glGenerateMipmap(target);
      if (check_error()) {
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
    if (check_error()) {
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
