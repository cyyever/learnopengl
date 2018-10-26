#include <glad/glad.h>
#include <gsl/gsl>

#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "program.hpp"
#include "texture.hpp"

namespace {
constexpr int screen_width = 800;
constexpr int screen_height = 600;

opengl::camera cube_camera({0.0f, 0.0f, 3.0f}, {0.0f, 1.0f, 0.0f},
                           {0.0f, 0.0f, -1.0f});
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = screen_width / 2, lastY = screen_height / 2;
bool firstMouse = false;

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback([[maybe_unused]] GLFWwindow *window, double xpos,
                    double ypos) {
  if (firstMouse) // this bool variable is initially set to true
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates range from bottom to top
  lastX = xpos;
  lastY = ypos;
  cube_camera.lookat(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]] GLFWwindow *window,
                     [[maybe_unused]] double xoffset, double yoffset) {
  cube_camera.add_fov(yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cube_camera.move(opengl::camera::movement::forward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cube_camera.move(opengl::camera::movement::backward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cube_camera.move(opengl::camera::movement::left, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cube_camera.move(opengl::camera::movement::right, deltaTime);
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

  GLFWwindow *window = glfwCreateWindow(screen_width, screen_height,
                                        "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

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
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

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

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  opengl::program prog;
  if (!prog.attach_shader(
          GL_VERTEX_SHADER,
          "#version 330 core\n"
          "layout (location = 0) in vec3 aPos;\n"
          "layout (location = 1) in vec2 aTexCoord;\n"
          "uniform mat4 model;\n"
          "uniform mat4 view;\n"
          "uniform mat4 projection;\n"
          "out vec2 TexCoord;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
          "    TexCoord = aTexCoord;\n"
          "}\n")) {
    return -1;
  }

  if (!prog.attach_shader(
          GL_FRAGMENT_SHADER,
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
          "    FragColor = mix(texture(texture1, TexCoord), texture(texture2, "
          "TexCoord), 0.2);\n"
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

  glEnable(GL_DEPTH_TEST);

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    auto view = cube_camera.get_view_matrix();
    if (!prog.set_uniform_by_callback("view", [&view](auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
        })) {
      return -1;
    }
    auto projection = glm::perspective(
        cube_camera.get_fov(), static_cast<float>(screen_width) / screen_height,
        0.1f, 100.0f);

    if (!prog.set_uniform_by_callback("projection", [&projection](
                                                        auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));
        })) {
      return -1;
    }

    for (size_t i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
      glm::mat4 model(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      if (!prog.set_uniform_by_callback("model", [&model](auto location) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
          })) {
        return -1;
      }

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
