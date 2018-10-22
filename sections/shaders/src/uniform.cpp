#include <glad/glad.h>
#include <gsl/gsl>

#include <GLFW/glfw3.h>
#include <iostream>

#include "program.hpp"

namespace {
void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
}

} // namespace

int main() {
  if (glfwInit() != GL_TRUE) {
    std::cerr << "glfwInit failed" << std::endl;
  }
  auto cleanup = gsl::finally([]() { glfwTerminate(); });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        (GLvoid *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  opengl::program prog;
  if (!prog.attach_shader(
          GL_VERTEX_SHADER,
          "#version 330 core\n"
          "layout (location = 0) in vec3 aPos;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
          "}\n")) {
    return -1;
  }

  if (!prog.attach_shader(
          GL_FRAGMENT_SHADER,
          "#version 330 core\n"
          "out vec4 FragColor;\n"
          "  \n"
          "uniform vec4 ourColor; // we set this variable in the OpenGL code.\n"
          "\n"
          "void main()\n"
          "{\n"
          "    FragColor = ourColor;\n"
          "}\n")) {
    return -1;
  }

  if (!prog.use()) {
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    // update the uniform color
    auto timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    if (!prog.set_uniform_by_callback("ourColor", [greenValue](auto location) {
          glUniform4f(location, 0.0f, greenValue, 0.0f, 1.0f);
        })) {
      return -1;
    }

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
