#include <glad/glad.h>
#include <gsl/gsl>

#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "program.hpp"
#include "texture.hpp"

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

  opengl::texture texture1(GL_TEXTURE_2D, GL_TEXTURE0,
                           "resource/container.jpg");

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  texture1.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture1.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture1.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture1.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture1.use()) {
    return -1;
  }

  opengl::texture texture2(GL_TEXTURE_2D, GL_TEXTURE1,
                           "resource/awesomeface.png");

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  texture2.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture2.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture2.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture2.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture2.use()) {
    return -1;
  }

  float vertices[] = {
      // positions           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };

  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  opengl::program prog;
  if (!prog.attach_shader(GL_VERTEX_SHADER,
                          "#version 330 core\n"
                          "layout (location = 0) in vec3 aPos;\n"
                          "layout (location = 1) in vec2 aTexCoord;\n"
                          "uniform mat4 transform;\n"
                          "out vec2 TexCoord;\n"
                          "\n"
                          "void main()\n"
                          "{\n"
                          "    gl_Position = transform * vec4(aPos, 1.0);\n"
                          "    TexCoord = aTexCoord;\n"
                          "}\n")) {
    return -1;
  }

  if (!prog.attach_shader(GL_FRAGMENT_SHADER,
                          "#version 330 core\n"
                          "out vec4 FragColor;\n"
                          "  \n"
                          "in vec2 TexCoord;\n"
                          "\n"
                          "uniform sampler2D texture1;\n"
                          "uniform sampler2D texture2;\n"
                          "\n"
                          "void main()\n"
                          "{\n"
                          "    FragColor = mix(texture(texture1, TexCoord), "
                          "texture(texture2, TexCoord), 0.2);\n"
                          "}\n")) {
    return -1;
  }

  if (!prog.set_uniform("texture1", texture1)) {
    return -1;
  }
  if (!prog.set_uniform("texture2", texture2)) {
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

    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans =
        glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    if (!prog.set_uniform_by_callback("transform", [&trans](auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(trans));
        })) {
      return -1;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
