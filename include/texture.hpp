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

template <GLenum target> class texture final {
  static_assert(target == GL_TEXTURE_2D, "unsupported texture target");

public:
  struct extra_config {
    extra_config() : generate_mipmap{true}, flip_y{true} {}
    bool generate_mipmap;
    bool flip_y;
  };

  enum class type : int {
    diffuse = 1,
    specular,
  };

  explicit texture(std::filesystem::path image, extra_config config = {}) {

    if (!std::filesystem::exists(image)) {
      throw_exception(std::string("no image ") + image.string());
    }

    glGenTextures(1, texture_id.get());
    if (check_error()) {
      throw_exception("glBindTexture failed");
    }

    glBindTexture(target, *texture_id);
    if (check_error()) {
      throw_exception("glBindTexture failed");
    }

    stbi_set_flip_vertically_on_load(config.flip_y);

    int width, height, channel;
    auto *data =
        stbi_load(image.string().c_str(), &width, &height, &channel, 0);
    if (!data) {
      throw_exception(std::string("stbi_load ") + image.string() + " failed");
    }
    auto cleanup = gsl::finally([data]() { stbi_image_free(data); });
    GLenum format = 0;
    if (channel == 3) {
      format = GL_RGB;
    } else if (channel == 4) {
      format = GL_RGBA;
    } else {
      throw_exception("unsupported channels");
    }

    if constexpr (target == GL_TEXTURE_2D) {
      glTexImage2D(target, 0, GL_RGBA, width, height, 0, format,
                   GL_UNSIGNED_BYTE, data);
      if (check_error()) {
        throw_exception("glTexImage2D failed");
      }
    }

    if (config.generate_mipmap) {
      glGenerateMipmap(target);
      if (check_error()) {
        throw_exception("glGenerateMipmap failed");
      }
    }
  }

  texture(const texture &) = default;
  texture &operator=(const texture &) = default;

  texture(texture &&) noexcept = default;
  texture &operator=(texture &&) noexcept = default;

  ~texture() noexcept = default;

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

public:
  bool use(GLenum unit) noexcept {
    glActiveTexture(unit);
    if (check_error()) {
      std::cerr << "glActiveTexture failed" << std::endl;
      return false;
    }
    glBindTexture(target, *texture_id);
    if (check_error()) {
      std::cerr << "glBindTexture failed" << std::endl;
      return false;
    }
    return true;
  }

private:
  std::shared_ptr<GLuint> texture_id{new GLuint(0), [](GLuint *ptr) {
                                       glDeleteTextures(1, ptr);
                                       delete ptr;
                                     }};
};

using texture_2D = texture<GL_TEXTURE_2D>;
} // namespace opengl
