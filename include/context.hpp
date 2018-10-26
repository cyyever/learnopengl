#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <functional>
#include <gsl/gsl>
#include <iostream>
#include <optional>

namespace opengl {

class context final {
public:
  class window final {

  public:
    window() = delete;
    window(const window &) = delete;
    window &operator=(const window &) = delete;

    window(window &&rhs) {
	operator=(std::move(rhs));
    }
    window &operator=(window &&rhs) {
      if(this !=&rhs) {
	if(handler) {
	   glfwDestroyWindow(handler);
	   handler=nullptr;
	}
	std::swap(handler,rhs.handler);
      }
      return *this;
    }

    ~window() {
      if(handler) {
	glfwDestroyWindow(handler);
      }
    }

    operator GLFWwindow *() const { return handler; }

  private:
    friend class context;
    window(GLFWwindow *handler_) : handler(handler_) {}

  private:
    GLFWwindow *handler{nullptr};
  };
  static std::optional<window> create(int window_width, int window_height,
                                      const std::string &title) {

    if (glfwInit() != GL_TRUE) {
      std::cerr << "glfwInit failed" << std::endl;
      return {};
    }

    glfwSetErrorCallback([](int error, const char *description) {
      std::cerr << "GLFW error code:" << error << " description:" << description
                << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto glfw_window = glfwCreateWindow(window_width, window_height,
                                        title.c_str(), nullptr, nullptr);
    if (glfw_window == nullptr) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      return {};
    }
    window win(glfw_window);

    glfwMakeContextCurrent(win);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cerr << "Failed to initialize GLAD" << std::endl;
      return {};
    }

    glEnable(GL_DEPTH_TEST);

    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(debug_callback, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                            nullptr, GL_TRUE);
    }
    return {std::move(win)};
  }

private:
  static void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length,
                                      const GLchar *message,
                                      const void *userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
      return;

    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source: Other";
      break;
    }
    std::cout << std::endl;

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type: Other";
      break;
    }
    std::cout << std::endl;

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Severity: notification";
      break;
    }
    std::cout << std::endl;
  }

private:
  inline static gsl::final_action cleanup{
      gsl::finally([]() {
	  glfwTerminate();
	  })};
};

} // namespace opengl
