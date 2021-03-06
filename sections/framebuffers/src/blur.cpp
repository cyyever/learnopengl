#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_cpp/buffer.hpp"
#include "opengl_cpp/camera.hpp"
#include "opengl_cpp/context.hpp"
#include "opengl_cpp/frame_buffer.hpp"
#include "opengl_cpp/model.hpp"
#include "opengl_cpp/program.hpp"

namespace {
constexpr int screen_width = 1280;
constexpr int screen_height = 740;

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

  opengl::texture_2D cube_texture("resource/container.jpg");

  opengl::texture_2D plant_texture("resource/metal.png");

  opengl::program scene_prog;
  if (!scene_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/scene.vs")) {
    return -1;
  }

  if (!scene_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/scene.fs")) {
    return -1;
  }

  opengl::program quad_prog;
  if (!quad_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/quad.vs")) {
    return -1;
  }

  if (!quad_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/blur.fs")) {
    return -1;
  }

  float quad_vertices[] = {// positions   // texCoords
                           -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                           0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                           -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                           1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  opengl::vertex_array quad_VAO;
  opengl::buffer<GL_ARRAY_BUFFER, float> quad_VBO;

  if (!quad_VBO.write(quad_vertices)) {
    return -1;
  }
  if (!quad_VBO.vertex_attribute_pointer_simple_offset(0, 2, 4, 0)) {
    return -1;
  }
  if (!quad_VBO.vertex_attribute_pointer_simple_offset(1, 2, 4, 2)) {
    return -1;
  }

  quad_prog.set_vertex_array(quad_VAO);

  opengl::frame_buffer scene_frame_buffer;
  opengl::depth_stencil_render_buffer RBO(screen_width, screen_height);
  opengl::texture_2D scene_texture(screen_width, screen_height);
  scene_frame_buffer.add_color_attachment(scene_texture);
  scene_frame_buffer.add_depth_and_stencil_attachment(RBO);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (!scene_frame_buffer.use()) {
      return -1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto view = model_camera.get_view_matrix();
    auto projection = glm::perspective(
        model_camera.get_fov(),
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

    if (!opengl::frame_buffer::use_default()) {
      return -1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    if (!quad_prog.set_uniform("screenTexture", scene_texture)) {
      return -1;
    }

    if (!quad_prog.use()) {
      return -1;
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
