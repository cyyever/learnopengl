#include <glad/glad.h>
#include <gsl/gsl>

#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
  auto cleanup=gsl::finally([]() { glfwTerminate(); });

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

  stbi_set_flip_vertically_on_load(true);

  GLuint texture1;
  glGenTextures(1, &texture1);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);

  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  auto *data = stbi_load("resource/container.jpg", &width, &height,&nrChannels, 0); 
  if(!data) {
    std::cout << "stbi_load failed" << std::endl;
    return -1;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  GLuint texture2;
  glGenTextures(1, &texture2);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);

  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  data = stbi_load("resource/awesomeface.png", &width, &height,&nrChannels, 0);
  if(!data) {
    std::cout << "stbi_load failed" << std::endl;
    return -1;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

float vertices[] = {
    // positions           // texture coords
     0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,    1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,    0.0f, 1.0f    // top left
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1); 

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
  if (vertexShader == 0) {
    std::cout << "glCreateShader failed" << std::endl;
    return -1;
  }

  const GLchar *vertexShaderSource =
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
"}\n";

  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  GLint success = 0;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "glCompileShader failed" << infoLog << std::endl;
    return -1;
  }

  auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (fragmentShader == 0) {
    std::cout << "glCreateShader failed" << std::endl;
    return -1;
  }

  const GLchar *fragmentShaderSource =
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
"    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);\n"
"}\n";

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "glCompileShader failed" << infoLog << std::endl;
    return -1;
  }

  auto shaderProgram = glCreateProgram();
  if (shaderProgram == 0) {
    std::cout << "glCreateProgram failed" << std::endl;
    return -1;
  }
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "glGetProgramInfoLog failed" << infoLog << std::endl;
    return -1;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // set it manually
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1); // set it manually


  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glUseProgram(shaderProgram);

    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    glm::mat4 view(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f),  800.0f/600, 0.1f, 100.0f);
    auto modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    auto viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    auto projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
