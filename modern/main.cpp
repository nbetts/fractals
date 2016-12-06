#include "main.hpp"

GLFWwindow* window;
GLint frameWidth, frameHeight;
GLuint vbo, vao, ebo, lightVao;
GLuint boxShaderID, lampBoxShaderID;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f),
              glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.0f, 0.2f, 45.0f);

// keyboard variables
GLuint keyPressed[256];

// mouse variables
GLfloat lastX = WINDOW_WIDTH / 2.0f;
GLfloat lastY = WINDOW_HEIGHT / 2.0f;
GLuint settingUp = true;

// time variables
GLfloat currentTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat deltaTime = 0.0f;

// lighting variables
glm::vec3 lightPosition(1.2f, 1.0f, 2.0);

GLfloat vertices[] = {
  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,
   0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,

  -0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,
   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,
  -0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,

  -0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,
  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,
  -0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.2f,

   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,
   0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,

  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,
   0.5f, -0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
   0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f, -0.5f, -0.5f,   1.0f, 0.3f, 0.2f,

  -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,
   0.5f,  0.5f,  0.5f,   1.0f, 0.8f, 0.2f,
  -0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.2f,
  -0.5f,  0.5f, -0.5f,   1.0f, 0.5f, 0.2f,
};

GLuint indices[] = {
    0, 1, 2,
    0, 2, 3
};

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

/**
 * Update the camera's position relative to the world.
 */
GLvoid move()
{
  camera.setMovementSpeed(2.0f * deltaTime);
  GLfloat movementSpeed = camera.getMovementSpeed();

  if (keyPressed[GLFW_KEY_W]) {
    camera.position += camera.front * movementSpeed;
  }
  if (keyPressed[GLFW_KEY_S]) {
    camera.position -= camera.front * movementSpeed;
  }
  if (keyPressed[GLFW_KEY_A]) {
    camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) *
                       movementSpeed;
  }
  if (keyPressed[GLFW_KEY_D]) {
    camera.position += glm::normalize(glm::cross(camera.front, camera.up)) *
                       movementSpeed;
  }
}

/**
 * Listen for keyboard events.
 */
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode)
{
  // Store the action state of the given key.
  keyPressed[key] = action;

  switch(key) {
    case GLFW_KEY_ESCAPE:
      if (keyPressed[key]) {
        glfwSetWindowShouldClose(window, GL_TRUE);
      }
      break;
  }
}

/**
 * Listen for mouse events.
 */
GLvoid mouseMovement(GLFWwindow* window, GLdouble x, GLdouble y)
{
  if (settingUp) {
    lastX = x;
    lastY = y;
    settingUp = false;
  }

  camera.setYaw(camera.getYaw() + (x - lastX) *
                camera.getTurnSensitivity());
  camera.setPitch(camera.getPitch() + (lastY - y) *
                  camera.getTurnSensitivity());
  GLfloat yaw = camera.getYaw();
  GLfloat pitch = camera.getPitch();

  lastX = x;
  lastY = y;

  // update the camera's yaw and pitch.
  camera.front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  camera.front.y = sin(glm::radians(pitch));
  camera.front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  camera.front = glm::normalize(camera.front);
}

/**
 * Listen for mouse scroll events.
 */
GLvoid mouseScroll(GLFWwindow* window, GLdouble x, GLdouble y)
{
  camera.setFov(camera.getFov() + y);
}

/**
 * Update the time variables.
 */
GLvoid updateTime()
{
  lastTime = currentTime;
  currentTime = glfwGetTime();
  deltaTime = currentTime - lastTime;
}

/**
 * Run the close event loop. This is where elements are drawn and window
 * events are polled.
 */
GLvoid runMainLoop()
{
  using namespace glm;

  // Create the view and projection (perspective) matrix.
  mat4 view, projection;

  while(!glfwWindowShouldClose(window))
  {
    // Update the time variables.
    updateTime();

    // Listen for events from the window.
    glfwPollEvents();

    // Update the project (perspective) matrix.
    projection = perspective(radians(camera.getFov()),
                             (GLfloat)frameWidth / (GLfloat)frameHeight,
                             0.1f, 100.0f);

    // Update the camera view matrix.
    move();
    view = camera.getLookAtMatrix();

    // Clear the screen.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the current shader program.
    glUseProgram(boxShaderID);
    GLuint lightColorLoc = glGetUniformLocation(boxShaderID, "lightColor");
    glUniform3f(lightColorLoc, 0.2f, 1.0f, 1.0f);

    // Transform the shader program's vertices with the model, view and
    // projection matrices.
    GLuint modelLoc = glGetUniformLocation(boxShaderID, "model");
    GLuint viewLoc = glGetUniformLocation(boxShaderID, "view");
    GLuint projectionLoc = glGetUniformLocation(boxShaderID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

    // Draw functions.
    glBindVertexArray(vao);
    mat4 model;

    for (GLuint i = 0; i < 10; i++) {
      model = mat4();
      model = translate(model, cubePositions[i]);

      GLfloat angle = i * 20.0f;
      model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
      model = rotate(model, currentTime * radians(-60.0f),
                     vec3(1.0f, 0.3f, 0.5f));
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
      // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // light
    glUseProgram(lampBoxShaderID);
    glBindVertexArray(lightVao);

    modelLoc = glGetUniformLocation(boxShaderID, "model");
    viewLoc = glGetUniformLocation(boxShaderID, "view");
    projectionLoc = glGetUniformLocation(boxShaderID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

    model = mat4();
    model = translate(model, lightPosition);
    model = scale(model, vec3(0.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

    // Draw the light object
    glBindVertexArray(lightVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
  }
}

/**
 * Initialise the graphics libraries and window.
 */
GLvoid initialiseGraphics(GLint argc, GLchar* argv[])
{
  GLint majorVersion, minorVersion, revision;
  GLint isFullscreen = argc > 1 ? atoi(argv[1]) : false;

  // Initialise GLFW.
  glfwInit();

  // Initialise the window and force the use of modern OpenGL (v. >= 3.0).
  glfwGetVersion(&majorVersion, &minorVersion, &revision);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWmonitor* fullscreen = isFullscreen ? glfwGetPrimaryMonitor() : nullptr;
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Modern OpengGL",
                            fullscreen, nullptr);

  glfwMakeContextCurrent(window);

  // Enable keyboard and mouse input.
  glfwSetKeyCallback(window, keyboard);
  glfwSetCursorPosCallback(window, mouseMovement);
  glfwSetScrollCallback(window, mouseScroll);

  // Initialise GLEW.
  glewExperimental = GL_TRUE;
  glewInit();

  // Define the viewport dimensions.
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);  
  glViewport(0, 0, frameWidth, frameHeight);

  // Set extra options.
  glEnable(GL_DEPTH_TEST);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/**
 * Terminate the application gracefully by deleting objects and properly
 * closing the window.
 */
GLvoid terminateGraphics()
{
  glDeleteProgram(boxShaderID);
  glDeleteProgram(lampBoxShaderID);
  glDeleteVertexArrays(1, &vao);
  glDeleteVertexArrays(1, &lightVao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glfwTerminate();
}

/**
 * Main method.
 */
GLint main(GLint argc, GLchar* argv[])
{
  // I have to reset these camera values because c++ is being shitty.
  camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
  camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
  camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Initialise the graphics environment.
  initialiseGraphics(argc, argv);

  // Load the vertex and fragment shaders into a shader program.
  Shader box("shaders/box.vert", "shaders/box.frag");
  boxShaderID = box.getProgramID();
  Shader lampBox("shaders/box.vert", "shaders/lamp_box.frag");
  lampBoxShaderID = lampBox.getProgramID();
  
   // Create the vertex array and buffer objects.
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  // Bind the vao and the  vertices to the vbo.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
               vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
               indices, GL_STATIC_DRAW);

  // Add layout attributes to the shader.
  const GLint attributeCount = 2;
  const GLchar* attributeNames[attributeCount] = {"position", "color"};
  GLint attributeSizes[attributeCount] = {3, 3};
  box.setAttributes(attributeCount, attributeNames, attributeSizes);
  glBindVertexArray(0);

  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(lightVao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  lampBox.setAttributes(attributeCount, attributeNames, attributeSizes);
  glBindVertexArray(0);

  // Unbind the vertex buffer and array objects.
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Run the graphics loop.
  runMainLoop();

  // Close the application gracefully.
  terminateGraphics();

  return 0;
}
