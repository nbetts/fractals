/**
 * [Program description]
 */

#include "src/main.hpp"

// System info
GLFWwindow* window;
GLint frameWidth, frameHeight;
GLfloat aspectRatio;

// Buffer and shader info.
GLuint vao[1], vbo[1], ebo[1], shader[1];

// environment info
std::map<std::string, GLfloat> env;

// keyboard info
GLuint keyPressed[256];

// mouse info
GLfloat lastMouseX = DEFAULT_WINDOW_WIDTH / 2.0f;
GLfloat lastMouseY = DEFAULT_WINDOW_HEIGHT / 2.0f;
GLuint initialMouseMovement = true;

// time info
GLfloat currentTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat deltaTime = 0.0f;

Camera camera(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
              -90.0f, 0.0f, 5.0f, 0.2f, 45.0f);
Fractal fractal(1, 0.0f, 0.0f, glm::vec3(0.0f));
GLfloat backgroundColour[3];

/**
 * Listen for keyboard events.
 */
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode)
{
  // Store the action state of the given key.
  keyPressed[key] = action;

  if (action != GLFW_PRESS) {
    return;
  }

  switch(key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
  }
}

/**
 * Listen for mouse events.
 */
GLvoid mouseMovement(GLFWwindow* window, GLdouble x, GLdouble y)
{
  // Set the last x and y values for the mouse's initial movement capture.
  if (initialMouseMovement) {
    lastMouseX = x;
    lastMouseY = y;
    initialMouseMovement = false;
  }

  // Update the camera's yaw and pitch.
  GLfloat yaw = camera.getYaw() + (x - lastMouseX) *
                camera.getTurnSensitivity();
  GLfloat pitch = camera.getPitch() + (lastMouseY - y) *
                  camera.getTurnSensitivity();

  camera.updateOrientation(yaw, pitch);
  lastMouseX = x;
  lastMouseY = y;
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
 * Update any camera atrributes before rendering the scene.
 */
GLvoid updateCamera()
{
  GLfloat distance = camera.getMovementSpeed() * deltaTime;

  if (keyPressed[GLFW_KEY_W]) {
    camera.updatePosition(Camera::FORWARD, distance);
  }
  if (keyPressed[GLFW_KEY_S]) {
    camera.updatePosition(Camera::BACKWARD, distance);
  }
  if (keyPressed[GLFW_KEY_A]) {
    camera.updatePosition(Camera::LEFT, distance);
  }
  if (keyPressed[GLFW_KEY_D]) {
    camera.updatePosition(Camera::RIGHT, distance);
  }

  camera.projection = glm::perspective(glm::radians(camera.getFov()),
                                       aspectRatio, 0.01f, 1000.0f);
  camera.view = camera.getLookAtMatrix();
}

/**
 * Draw the fractal.
 */
GLvoid drawFractal()
{
  using namespace glm;

  mat4 modelMatrix;
  GLfloat scaleFactor = 100.0f;
  GLfloat yOffset = fractal.getY(fractal.size / 2, fractal.size / 2) +
                          (1.5f / scaleFactor);

  glUseProgram(shader[Shader::FRACTAL]);
  
  // Transform the shader program's vertices with the model, view and
  // projection matrices.
  GLuint model = glGetUniformLocation(shader[Shader::FRACTAL], "model");
  GLuint view = glGetUniformLocation(shader[Shader::FRACTAL], "view");
  GLuint projection = glGetUniformLocation(shader[Shader::FRACTAL],
                                           "projection");
  glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(camera.view));
  glUniformMatrix4fv(projection, 1, GL_FALSE, value_ptr(camera.projection));

  // Draw functions.
  glBindVertexArray(vao[Shader::FRACTAL]);
  modelMatrix = scale(modelMatrix, vec3(scaleFactor, scaleFactor, scaleFactor));
  modelMatrix = translate(modelMatrix, vec3(-0.5f, -yOffset, -0.5f));
  glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(modelMatrix));

  glDrawElements(GL_TRIANGLES, fractal.indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

/**
 * Run the close event loop. This is where elements are drawn and window
 * events are polled.
 */
GLvoid runMainLoop()
{
  using namespace glm;

  while(!glfwWindowShouldClose(window))
  {
    // Update the time variables.
    updateTime();

    // Listen for events from the window.
    glfwPollEvents();

    // Update the camera attributes.
    updateCamera();

    // Clear the screen.
    glClearColor(backgroundColour[0],
                 backgroundColour[1],
                 backgroundColour[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw functions.
    drawFractal();

    glfwSwapBuffers(window);
  }
}

/**
 * Setup the buffer objects and shaders for each object type.
 */
GLvoid initialiseBuffersAndShaders()
{
  for (GLuint i = Shader::FRACTAL; i != Shader::NONE; i++) {
    glGenVertexArrays(1, &vao[i]);
    glGenBuffers(1, &vbo[i]);
    glGenBuffers(1, &ebo[i]);
  }

  // Load the vertex and fragment shaders into a shader program.
  Shader fractalShader("src/shaders/fractal.vert", "src/shaders/fractal.frag");
  shader[Shader::FRACTAL] = fractalShader.getProgramID();
}

/**
 * Generate the fractal and load the vertex/index data into the buffers.
 */
GLvoid updateFractalBuffer()
{
  fractal.generate();

  // Bind the vao, vbo and ebo with the fractal vertex data.
  glBindVertexArray(vao[Shader::FRACTAL]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[Shader::FRACTAL]);
  glBufferData(GL_ARRAY_BUFFER, fractal.vertexCount * 6 * sizeof(GLfloat),
               fractal.vertexData, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[Shader::FRACTAL]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, fractal.indexCount * sizeof(GLuint),
               fractal.indexData, GL_STATIC_DRAW);

  addVertexAttributes(shader[Shader::FRACTAL]);

  // Unbind the vao, vbo and ebo.
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * Add vertex layout attributes to the given shader.
 */
GLvoid addVertexAttributes(GLuint shaderID)
{
  // Vertex attributes. These are consistent accross all shaders used.
  const GLint attributeCount = 2;
  const GLchar* attributeNames[attributeCount] = {"position", "color"};
  GLint attributeSizes[attributeCount] = {3, 3};

  GLint i, stride = 0, offset = 0;

  for (i = 0; i < attributeCount; i++) {
    stride += attributeSizes[i];
  }

  for (i = 0; i < attributeCount; i++) {
    GLint attribute = glGetAttribLocation(shaderID,
                                          attributeNames[i]);
    glVertexAttribPointer(attribute, attributeSizes[i], GL_FLOAT,
                          GL_FALSE, stride * sizeof(GLfloat),
                          (GLvoid*)(offset * sizeof(GLfloat)));
    glEnableVertexAttribArray(attribute);
    offset += attributeSizes[i];
  }
}

/**
 * Initialise the graphics libraries and window.
 */
GLvoid initialiseGraphics(GLint argc, GLchar* argv[])
{
  GLint majorVersion, minorVersion, revision;
  // GLint isFullscreen = argc > 1 ? atoi(argv[1]) : false;
  GLint isFullscreen = env["isFullscreenEnabled"];

  // Initialise GLFW.
  glfwInit();

  // Initialise the window and force the use of modern OpenGL (v. >= 3.0).
  glfwGetVersion(&majorVersion, &minorVersion, &revision);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLuint width = DEFAULT_WINDOW_WIDTH;
  GLuint height = DEFAULT_WINDOW_HEIGHT;
  GLFWmonitor* monitor = nullptr;

  if (isFullscreen) {
    const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
    width = videoMode->width;
    height = videoMode->height;
    monitor = glfwGetPrimaryMonitor();
  }
  window = glfwCreateWindow(width, height, "Diamond-square Fractal",
                            monitor, nullptr);

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
  aspectRatio = (GLfloat)frameWidth / (GLfloat)frameHeight;
  glViewport(0, 0, frameWidth, frameHeight);

  // Set extra options.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the clear colour value.
  backgroundColour[0] = env["backgroundColourRed"];
  backgroundColour[1] = env["backgroundColourGreen"];
  backgroundColour[2] = env["backgroundColourBlue"];
}

/**
 * Terminate the application gracefully by deleting objects and properly
 * closing the window.
 */
GLvoid terminateGraphics()
{
  for (GLuint i = Shader::FRACTAL; i != Shader::NONE; i++) {
    glDeleteProgram(shader[i]);
    glDeleteVertexArrays(1, &vao[i]);
    glDeleteBuffers(1, &vbo[i]);
    glDeleteBuffers(1, &ebo[i]);
  }

  glfwTerminate();
}

/**
 * Main method.
 */
GLint main(GLint argc, GLchar* argv[])
{
  srand(time(nullptr));

  // Read in the profile.
  const GLchar* profile = (argc >= 2) ? argv[1] : "profile.txt";
  env = readProfile(profile);

  // Instantiate the fractal.
  fractal = Fractal(env["fractalDepth"],
                    env["fractalYRange"],
                    env["fractalYDeviance"],
                    glm::vec3(env["fractalColourRed"],
                              env["fractalColourGreen"],
                              env["fractalColourBlue"]));

  // Reset the camera vectors.
  camera.position = glm::vec3(0.5f, 0.0f, 5.0f);
  camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
  camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Initialise the graphics environment.
  initialiseGraphics(argc, argv);
  
  // Initialise the buffers and shaders.
  initialiseBuffersAndShaders();

  // Push the vertex data into the buffers.
  updateFractalBuffer();

  // Run the graphics loop.
  runMainLoop();

  // Close the application gracefully.
  terminateGraphics();

  return 0;
}
