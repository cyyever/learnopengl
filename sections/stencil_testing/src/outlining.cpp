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

  opengl::program scene_prog;
  if (!scene_prog.attach_shader_file(GL_VERTEX_SHADER,
                                     "shader/depth_testing.vs")) {
    return -1;
  }

  if (!scene_prog.attach_shader_file(GL_FRAGMENT_SHADER,
                                     "shader/depth_testing.fs")) {
    return -1;
  }

  opengl::program border_prog;
  if (!border_prog.attach_shader_file(GL_VERTEX_SHADER,
                                      "shader/depth_testing.vs")) {
    return -1;
  }

  if (!border_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/border.fs")) {
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

    scene_prog.set_vertex_array(plane_VAO);

    if (!scene_prog.set_uniform("texture1", plant_texture)) {
      return -1;
    }

    if (!scene_prog.use()) {
      return -1;
    }
    glStencilMask(0x00); // make sure we don't update the stencil buffer while
                         // drawing the floor
    glDrawArrays(GL_TRIANGLES, 0, 6);

    auto const draw_two_cubes = [&cube_VAO, &cube_texture, &view, &projection](
                                    opengl::program &scene_prog, float scale) {
      scene_prog.set_vertex_array(cube_VAO);

      glm::mat4 model =
          glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
      model = glm::scale(model, glm::vec3(scale, scale, scale));
      if (!scene_prog.set_uniform("model", model)) {
        return false;
        ;
      }
      if (!scene_prog.use()) {
        return false;
        ;
      }
      glDrawArrays(GL_TRIANGLES, 0, 36);

      model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
      model = glm::scale(model, glm::vec3(scale, scale, scale));
      if (!scene_prog.set_uniform("model", model)) {
        return false;
        ;
      }
      if (!scene_prog.use()) {
        return false;
        ;
      }
      glDrawArrays(GL_TRIANGLES, 0, 36);
      return true;
    };

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    glStencilFunc(GL_ALWAYS, 1,
                  0xFF); // all fragments should update the stencil buffer
    glStencilMask(0xFF); // enable writing to the stencil buffer

    if (!scene_prog.set_uniform("texture1", cube_texture)) {
      return false;
      ;
    }

    if (!draw_two_cubes(scene_prog, 1.0f)) {
      return -1;
    }

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // disable writing to the stencil buffer
    glDisable(GL_DEPTH_TEST);

    if (!border_prog.set_uniform("view", view)) {
      return -1;
    }

    if (!border_prog.set_uniform("projection", projection)) {
      return -1;
    }

    if (!draw_two_cubes(border_prog, 1.1f)) {
      return -1;
    }

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
