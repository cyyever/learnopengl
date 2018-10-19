#include <glad/glad.h>
#include <gsl/gsl>

#include <GLFW/glfw3.h>
#include <iostream>

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

float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};    

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// position attribute
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// color attribute
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
  if (vertexShader == 0) {
    std::cout << "glCreateShader failed" << std::endl;
    return -1;
  }

  const GLchar *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0\n"
"layout (location = 1) in vec3 aColor; // the color variable has attribute position 1\n"
"  \n"
"out vec3 ourColor; // output a color to the fragment shader\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"    ourColor = aColor; // set ourColor to the input color we got from the vertex data\n"
"}       \n";

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
"out vec4 FragColor;  \n"
"in vec3 ourColor;\n"
"  \n"
"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0);\n"
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

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
   // be sure to activate the shader
    glUseProgram(shaderProgram);
  
    // update the uniform color
    auto timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
