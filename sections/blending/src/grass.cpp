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
constexpr int screen_width = 1280;
constexpr int screen_height = 740;

opengl::camera scene_camera({0.0f, 0.0f, 3.0f}, {0.0f, 1.0f, 0.0f},
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
  scene_camera.lookat(xoffset, yoffset);
}

void scroll_callback([[maybe_unused]] GLFWwindow *window,
                     [[maybe_unused]] double xoffset, double yoffset) {
  scene_camera.add_fov(yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    scene_camera.move(opengl::camera::movement::forward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    scene_camera.move(opengl::camera::movement::backward, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    scene_camera.move(opengl::camera::movement::left, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    scene_camera.move(opengl::camera::movement::right, deltaTime);
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

  float cube_vertices[] = {
      // positions          // texture Coords
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
  float plane_vertices[] = {
      // positions          // texture Coords (note we set these higher than 1
      // (together with GL_REPEAT as texture wrapping mode). this will cause the
      // floor texture to repeat)
      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
      0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
      0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

  float grass_vertices[] = {
      // positions         // texture Coords (swapped y coordinates because
      // texture is flipped upside down)
      0.0f, 0.5f, 0.0f, 0.0f,  1.0f, 0.0f, -0.5f, 0.0f,
      0.0f, 0.0f, 1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

      0.0f, 0.5f, 0.0f, 0.0f,  1.0f, 1.0f, -0.5f, 0.0f,
      1.0f, 0.0f, 1.0f, 0.5f,  0.0f, 1.0f, 1.0f};

  opengl::vertex_array cube_VAO;
  opengl::buffer<GL_ARRAY_BUFFER, float> cube_VBO;

  if (!cube_VBO.write(cube_vertices)) {
    return -1;
  }
  if (!cube_VBO.vertex_attribute_pointer_simple_offset(0, 3, 5, 0)) {
    return -1;
  }
  if (!cube_VBO.vertex_attribute_pointer_simple_offset(1, 2, 5, 3)) {
    return -1;
  }

  opengl::vertex_array plane_VAO;
  opengl::buffer<GL_ARRAY_BUFFER, float> plane_VBO;

  if (!plane_VBO.write(plane_vertices)) {
    return -1;
  }
  if (!plane_VBO.vertex_attribute_pointer_simple_offset(0, 3, 5, 0)) {
    return -1;
  }
  if (!plane_VBO.vertex_attribute_pointer_simple_offset(1, 2, 5, 3)) {
    return -1;
  }

  opengl::vertex_array grass_VAO;
  opengl::buffer<GL_ARRAY_BUFFER, float> grass_VBO;

  if (!grass_VBO.write(grass_vertices)) {
    return -1;
  }
  if (!grass_VBO.vertex_attribute_pointer_simple_offset(0, 3, 5, 0)) {
    return -1;
  }
  if (!grass_VBO.vertex_attribute_pointer_simple_offset(1, 2, 5, 3)) {
    return -1;
  }

  opengl::texture cube_texture(GL_TEXTURE_2D, GL_TEXTURE0,
                               "resource/marble.jpg");

  cube_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  cube_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  cube_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  cube_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  opengl::texture plant_texture(GL_TEXTURE_2D, GL_TEXTURE0,
                                "resource/metal.png");

  plant_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  plant_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  plant_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  plant_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  opengl::texture grass_texture(GL_TEXTURE_2D, GL_TEXTURE0,
                                "resource/grass.png");

  grass_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  grass_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  grass_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  grass_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  opengl::program scene_prog;
  if (!scene_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/blend.vs")) {
    return -1;
  }

  if (!scene_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/grass.fs")) {
    return -1;
  }

  std::vector<glm::vec3> vegetation;
  vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
  vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
  vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
  vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
  vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto view = scene_camera.get_view_matrix();
    auto projection = glm::perspective(
        scene_camera.get_fov(),
        static_cast<float>(screen_width) / screen_height, 0.1f, 100.0f);

    if (!scene_prog.set_uniform("view", view)) {
      return -1;
    }

    if (!scene_prog.set_uniform("projection", projection)) {
      return -1;
    }

    if (!scene_prog.set_uniform("model", glm::mat4(1.0f))) {
      return -1;
    }
    scene_prog.set_vertex_array(cube_VAO);

    if (!scene_prog.set_uniform("texture1", cube_texture)) {
      return -1;
    }

    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
    if (!scene_prog.set_uniform("model", model)) {
      return -1;
    }
    if (!scene_prog.use()) {
      return -1;
    }
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
    if (!scene_prog.set_uniform("model", model)) {
      return -1;
    }
    if (!scene_prog.use()) {
      return -1;
    }
    glDrawArrays(GL_TRIANGLES, 0, 36);

    scene_prog.set_vertex_array(plane_VAO);

    if (!scene_prog.set_uniform("texture1", plant_texture)) {
      return -1;
    }

    if (!scene_prog.use()) {
      return -1;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);

    scene_prog.set_vertex_array(grass_VAO);

    if (!scene_prog.set_uniform("texture1", grass_texture)) {
      return -1;
    }

    for (auto const &translate_vec : vegetation) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, translate_vec);
      if (!scene_prog.set_uniform("model", model)) {
        return -1;
      }
      if (!scene_prog.use()) {
        return -1;
      }
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
