#pragma once

#include <iostream>

namespace opengl {

// copy from <experimental/source_location> and modify some code to make clang
// success.

struct source_location {

  // 14.1.2, source_location creation
  static constexpr source_location current(
#if defined(__GNUG__) && !defined(__clang__)
      const char *__file = __builtin_FILE(), int __line = __builtin_LINE()
#else
      const char *__file = "unknown", int __line = 0
#endif
          ) noexcept {
    source_location __loc;
    __loc._M_file = __file;
    __loc._M_line = __line;
    return __loc;
  }

  constexpr source_location() noexcept : _M_file("unknown"), _M_line(0) {}

  // 14.1.3, source_location field access
  constexpr uint_least32_t line() const noexcept { return _M_line; }
  constexpr const char *file_name() const noexcept { return _M_file; }

private:
  const char *_M_file;
  uint_least32_t _M_line;
};

inline std::optional<GLenum>
check_error(source_location error_location = source_location::current()) {
  auto error_code = glGetError();
  if (error_code == GL_NO_ERROR) {
    return {};
  }

  std::string error_msg;
  switch (error_code) {
  case GL_INVALID_ENUM:
    error_msg = "INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    error_msg = "INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    error_msg = "INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    error_msg = "STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    error_msg = "STACK_UNDERFLOW";
    break;
  case GL_OUT_OF_MEMORY:
    error_msg = "OUT_OF_MEMORY";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    error_msg = "INVALID_FRAMEBUFFER_OPERATION";
    break;
  }
  std::cerr << error_location.file_name() << '(' << error_location.line() << ')'
            << ' ' << error_msg << std::endl;
  return {error_code};
}

} // namespace opengl
