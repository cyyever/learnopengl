#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "buffer.hpp"
#include "camera.hpp"
#include "context.hpp"
#include "program.hpp"

namespace {
constexpr int screen_width = 800;
constexpr int screen_height = 600;

opengl::camera cube_camera({0.0f, 0.0f, 3.0f}, {0.0f, 1.0f, 0.0f},
                           {0.0f, 0.0f, -1.0f});
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = screen_width / 2, lastY = screen_height / 2;
bool firstMouse = false;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  // first, configure the cube's VAO (and VBO)
  opengl::vertex_array cubeVAO;
  opengl::buffer<GL_ARRAY_BUFFER, GLfloat> VBO;
  if (!VBO.write(vertices)) {
    return -1;
  }

  if (!VBO.vertex_attribute_pointer_simple_offset(0, 3, 8, 0)) {
    return -1;
  }
  if (!VBO.vertex_attribute_pointer_simple_offset(1, 3, 8, 3)) {
    return -1;
  }
  if (!VBO.vertex_attribute_pointer_simple_offset(2, 3, 8, 6)) {
    return -1;
  }

  // second, configure the light's VAO (VBO stays the same; the vertices are the
  // same for the light object which is also a 3D cube)
  opengl::vertex_array lightVAO;

  if (!VBO.vertex_attribute_pointer_simple_offset(0, 3, 8, 0)) {
    return -1;
  }

  opengl::program container_prog;
  if (!container_prog.attach_shader_file(GL_VERTEX_SHADER,
                                         "shader/material.vs")) {
    return -1;
  }

  if (!container_prog.attach_shader_file(GL_FRAGMENT_SHADER,
                                         "shader/specular_map.fs")) {
    return -1;
  }

  glm::mat4 model(1.0f);
  if (!container_prog.set_uniform_by_callback("model", [&model](auto location) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
      })) {
    return -1;
  }

  if (!container_prog.set_uniform("material.shininess", 32.0f)) {
    return -1;
  }

  if (!container_prog.set_uniform("light.ambient", 0.5f, 0.5f, 0.5f)) {
    return -1;
  }
  if (!container_prog.set_uniform("light.diffuse", 0.2f, 0.2f, 0.2f)) {
    return -1;
  }

  if (!container_prog.set_uniform("light.specular", 1.0f, 1.0f, 1.0f)) {
    return -1;
  }

  if (!container_prog.set_uniform("light.position", lightPos)) {
    return -1;
  }

  opengl::texture texture1(GL_TEXTURE_2D, GL_TEXTURE0,
                           "resource/container2.png");

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  texture1.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture1.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture1.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture1.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture1.use()) {
    return -1;
  }

  if (!container_prog.set_uniform("material.diffuse", texture1)) {
    return -1;
  }

  opengl::texture texture2(GL_TEXTURE_2D, GL_TEXTURE1,
                           "resource/container2_specular.png");

  // set the texture wrapping/filtering options (on the currently bound texture
  // object)
  texture2.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  texture2.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  texture2.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture2.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (!texture2.use()) {
    return -1;
  }

  if (!container_prog.set_uniform("material.specular", texture2)) {
    return -1;
  }

  opengl::program lamp_prog;
  if (!lamp_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/lamp.vs")) {
    return -1;
  }

  if (!lamp_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/lamp.fs")) {
    return -1;
  }

  model = glm::mat4(1.0f);
  model = glm::translate(model, lightPos);
  model = glm::scale(model, glm::vec3(0.2f));

  if (!lamp_prog.set_uniform_by_callback("model", [&model](auto location) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
      })) {
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    auto view = cube_camera.get_view_matrix();
    auto projection = glm::perspective(
        cube_camera.get_fov(), static_cast<float>(screen_width) / screen_height,
        0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!cubeVAO.use()) {
      return -1;
    }

    if (!container_prog.use()) {
      return -1;
    }

    if (!container_prog.set_uniform("viewPos", cube_camera.get_position())) {
      return -1;
    }

    if (!container_prog.set_uniform_by_callback("view", [&view](auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
        })) {
      return -1;
    }

    if (!container_prog.set_uniform_by_callback(
            "projection", [&projection](auto location) {
              glUniformMatrix4fv(location, 1, GL_FALSE,
                                 glm::value_ptr(projection));
            })) {
      return -1;
    }

    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (!lightVAO.use()) {
      return -1;
    }

    if (!lamp_prog.use()) {
      return -1;
    }

    if (!lamp_prog.set_uniform_by_callback("view", [&view](auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
        })) {
      return -1;
    }

    if (!lamp_prog.set_uniform_by_callback("projection", [&projection](
                                                             auto location) {
          glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));
        })) {
      return -1;
    }

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
