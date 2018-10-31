#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "buffer.hpp"
#include "camera.hpp"
#include "context.hpp"
#include "model.hpp"
#include "program.hpp"

namespace {
constexpr int screen_width = 800;
constexpr int screen_height = 600;

opengl::camera model_camera({0.0f, 0.0f, 3.0f}, {0.0f, 1.0f, 0.0f},
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
  model_camera.lookat(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]] GLFWwindow *window,
                     [[maybe_unused]] double xoffset, double yoffset) {
  model_camera.add_fov(yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    model_camera.move(opengl::camera::movement::forward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    model_camera.move(opengl::camera::movement::backward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    model_camera.move(opengl::camera::movement::left, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    model_camera.move(opengl::camera::movement::right, deltaTime);
}

} // namespace

int main() {
  auto window_opt =
      opengl::context::create(screen_width, screen_height, "LearnOpenGL");
  if (!window_opt) {
    std::cerr << "create opengl context failed" << std::endl;
    return -1;
  }
  auto &window = window_opt.value();

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  opengl::model nanosuit_model("Debug/resource/nanosuit/nanosuit.obj");

  opengl::program model_prog;
  if (!model_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/model.vs")) {
    return -1;
  }

  if (!model_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/model.fs")) {
    return -1;
  }

  glm::mat4 model(1.0f);
  if (!model_prog.set_uniform("model", model)) {
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto view = model_camera.get_view_matrix();
    auto projection = glm::perspective(
        model_camera.get_fov(),
        static_cast<float>(screen_width) / screen_height, 0.1f, 100.0f);

    if (!model_prog.set_uniform("view", view)) {
      return -1;
    }

    if (!model_prog.set_uniform("projection", projection)) {
      return -1;
    }

    std::map<opengl::texture::type, std::vector<std::string>>
        texture_variable_names;
    texture_variable_names[opengl::texture::type::diffuse] = {
        "texture_diffuse1"};

    if (!nanosuit_model.draw(model_prog, texture_variable_names)) {
      return -1;
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
