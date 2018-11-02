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

  glEnable(GL_CULL_FACE);

/*
    Remember: to specify vertices in a counter-clockwise winding order you need to visualize the triangle
    as if you're in front of the triangle and from that point of view, is where you set their order.
    
    To define the order of a triangle on the right side of the cube for example, you'd imagine yourself looking
    straight at the right side of the cube, and then visualize the triangle and make sure their order is specified
    in a counter-clockwise order. This takes some practice, but try visualizing this yourself and see that this
    is correct.
*/

float cube_vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
};


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

  opengl::texture window_texture(GL_TEXTURE_2D, GL_TEXTURE0,
                                 "resource/checkerboard.png");

  window_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  window_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  window_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  window_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  opengl::program scene_prog;
  if (!scene_prog.attach_shader_file(GL_VERTEX_SHADER, "shader/cube.vs")) {
    return -1;
  }

  if (!scene_prog.attach_shader_file(GL_FRAGMENT_SHADER, "shader/cube.fs")) {
    return -1;
  }

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

    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f));
    if (!scene_prog.set_uniform("model", model)) {
      return -1;
    }

    scene_prog.set_vertex_array(cube_VAO);

    if (!scene_prog.set_uniform("texture1", window_texture)) {
      return -1;
    }

    if (!scene_prog.use()) {
      return -1;
    }
    glDrawArrays(GL_TRIANGLES, 0, 36);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
