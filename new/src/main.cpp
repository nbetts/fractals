/**
 * [Program description]
 */

#include "main.hpp"

// System info
GLFWwindow* window;
GLint frameWidth, frameHeight;
GLfloat aspectRatio;

// Buffer and shader info.
GLuint vao[1], vbo[1], ebo[1], fractalShader, normalShader;

// environment info
const GLchar* profile;
std::map<std::string, GLfloat> env;

// keyboard info
GLuint keyPressed[512];

// mouse info
GLfloat lastMouseX = DEFAULT_WINDOW_WIDTH / 2.0f;
GLfloat lastMouseY = DEFAULT_WINDOW_HEIGHT / 2.0f;
GLuint initialMouseMovement = true;

// time info
GLfloat currentTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat deltaTime = 0.0f;

// camera info
Camera camera(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f),
              0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

// lighting info
glm::vec3 lightPosition(0.0f);

// fractal info
Fractal fractal(0, 0.0f, 0.0f, glm::vec3(0.0f));
GLuint isPointLightingEnabled;
GLuint areFacesEnabled;
GLuint areNormalsEnabled;
GLuint isWireframeEnabled;
GLuint isCullingEnabled;
GLfloat shineValue = 1.0f;
GLfloat defaultNormalLength, normalLength;
glm::vec4 wireframeColour;

// misc. info
glm::vec3 backgroundColour(0.0f);

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
    case GLFW_KEY_1:
      initialiseEnvironment();
      generateFractal();
      updateFractalBuffer();
      break;
    case GLFW_KEY_SPACE:
      generateFractal();
      updateFractalBuffer();
      break;
    case GLFW_KEY_F:
      areFacesEnabled = !areFacesEnabled;
      env["areFacesEnabled"] = !env["areFacesEnabled"];
      break;
    case GLFW_KEY_C:
      isCullingEnabled = !isCullingEnabled;
      env["isCullingEnabled"] = isCullingEnabled;
      break;
    case GLFW_KEY_N:
      areNormalsEnabled = !areNormalsEnabled;
      env["areNormalsEnabled"] = areNormalsEnabled;
      break;
    case GLFW_KEY_X:
      isWireframeEnabled = !isWireframeEnabled;
      env["isWireframeEnabled"] = isWireframeEnabled;
      break;
    case GLFW_KEY_P:
      isPointLightingEnabled = !isPointLightingEnabled;
      env["isPointLightingEnabled"] = isPointLightingEnabled;
      break;
    case GLFW_KEY_Z:
      shineValue = -shineValue;
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
 * Initialise the environment properties, camera and fractal.
 */
GLvoid initialiseEnvironment()
{
  env = readProfile(profile);

  isPointLightingEnabled = env["isPointLightingEnabled"];
  lightPosition.x = env["lightPositionX"];
  lightPosition.y = env["lightPositionY"];
  lightPosition.z = env["lightPositionZ"];

  initialiseCamera();
  initialiseFractal();
}

/**
 * Initialise the camera attributes.
 */
GLvoid initialiseCamera()
{
  camera = Camera(glm::vec3(0.5f, 0.0f, 5.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f),
                  -90.0f, 0.0f,
                  env["cameraMovementSpeed"],
                  env["cameraTurnSensitivity"],
                  env["cameraFov"]);
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


  if (keyPressed[GLFW_KEY_U]) {
    lightPosition.x += 1.0f;
  }
  if (keyPressed[GLFW_KEY_J]) {
    lightPosition.x -= 1.0f;
  }
  if (keyPressed[GLFW_KEY_O]) {
    lightPosition.z += 1.0f;
  }
  if (keyPressed[GLFW_KEY_L]) {
    lightPosition.z -= 1.0f;
  }
  if (keyPressed[GLFW_KEY_I]) {
    lightPosition.y += 1.0f;
  }
  if (keyPressed[GLFW_KEY_K]) {
    lightPosition.y -= 1.0f;
  }
}

/**
 * Initialise the fractal attributes.
 */
GLvoid initialiseFractal()
{
  fractal = Fractal(env["fractalDepth"],
                    env["fractalYRange"],
                    env["fractalYDeviance"],
                    glm::vec3(env["fractalColourRed"],
                              env["fractalColourGreen"],
                              env["fractalColourBlue"]));
  areFacesEnabled = env["areFacesEnabled"];
  areNormalsEnabled = env["areNormalsEnabled"];
  isWireframeEnabled = env["isWireframeEnabled"];
  isCullingEnabled = env["isCullingEnabled"];
  normalLength = env["normalLength"];
  wireframeColour.r = env["wireframeColourRed"];
  wireframeColour.g = env["wireframeColourGreen"];
  wireframeColour.b = env["wireframeColourBlue"];
  wireframeColour.a = env["wireframeColourAlpha"];
}

/**
 * Generate the fractal.
 */
GLvoid generateFractal()
{
  fractal.generate();
  GLuint isModified = false;

  if (env["isSmoothingPositionsEnabled"]) {
    fractal.smoothPositions(fractal.createGaussianKernel(
                            env["smoothPositionsKernelSize"],
                            env["smoothPositionsSigmaValue"]));
    isModified = true;
  }
  if (env["isSmoothingPositionsEnabled"]) {
    fractal.smoothNormals(fractal.createBoxKernel(
                          env["smoothNormalsKernelSize"]));
    isModified = true;
  }
  if (env["isSmoothingPositionsEnabled"]) {
    fractal.smoothColours(fractal.createGaussianKernel(
                          env["smoothColoursKernelSize"],
                          env["smoothColoursSigmaValue"]));
    isModified = true;
  }
  if (env["isSmoothingPositionsEnabled"]) {
    fractal.addColourNoise(env["colourNoiseLevel"]);
    isModified = true;
  }

  if (isModified) {
    fractal.updateVertexData();
  }

  defaultNormalLength = 1.0f / (GLfloat)fractal.size;
}

/**
 * Draw the fractal.
 */
GLvoid drawFractal()
{
  using namespace glm;

  mat4 model;
  GLuint normalLengthLoc, facesLoc, wireframeLoc, wireframeColourLoc;
  GLuint matAmbientLoc, matDiffuseLoc, matSpecularLoc, matShineLoc;
  GLuint lightPositionLoc, lightAmbientLoc, lightDiffuseLoc, lightSpecularLoc;
  GLuint modelLoc, viewLoc, projectionLoc, viewPosLoc;

  GLfloat scaleFactor = 100.0f;
  GLfloat yOffset = fractal.getYPosition(fractal.size / 2, fractal.size / 2) +
                                         (2.0f / scaleFactor);
  model = scale(model, vec3(scaleFactor));
  model = translate(model, vec3(-0.5f, -yOffset, -0.5f));

  glUseProgram(fractalShader);

  // Transform the shader program's vertices with the model, view and
  // projection matrices.
  viewPosLoc  = glGetUniformLocation(fractalShader, "viewPosition");
  modelLoc      = glGetUniformLocation(fractalShader, "model");
  viewLoc       = glGetUniformLocation(fractalShader, "view");
  projectionLoc = glGetUniformLocation(fractalShader, "projection");

  glUniform4f(viewPosLoc, camera.position.x,
              camera.position.y, camera.position.z, 1.0f);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera.view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(camera.projection));

  // Material uniforms
  matAmbientLoc  = glGetUniformLocation(fractalShader, "material.ambient");
  matDiffuseLoc  = glGetUniformLocation(fractalShader, "material.diffuse");
  matSpecularLoc = glGetUniformLocation(fractalShader, "material.specular");
  matShineLoc    = glGetUniformLocation(fractalShader, "material.shininess"); 

  glUniform3f(matAmbientLoc,  0.0f, 0.0f, 0.0f);
  glUniform3f(matDiffuseLoc,  0.5f, 0.5f, 0.5f);
  glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
  glUniform1f(matShineLoc, shineValue);

  // Light uniforms
  lightPositionLoc = glGetUniformLocation(fractalShader, "light.position");
  lightAmbientLoc  = glGetUniformLocation(fractalShader, "light.ambient");
  lightDiffuseLoc  = glGetUniformLocation(fractalShader, "light.diffuse");
  lightSpecularLoc = glGetUniformLocation(fractalShader, "light.specular");

  glUniform4f(lightPositionLoc, lightPosition.x,
              lightPosition.y, lightPosition.z, isPointLightingEnabled);
  glUniform3f(lightAmbientLoc,  1.0f, 1.0f, 1.0f);
  glUniform3f(lightDiffuseLoc,  1.0f, 1.0f, 1.0f);
  glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

  facesLoc = glGetUniformLocation(fractalShader, "areFacesEnabled");
  wireframeLoc = glGetUniformLocation(fractalShader, "isWireframeEnabled");
  wireframeColourLoc = glGetUniformLocation(fractalShader, "wireframeColour");
  glUniform1f(facesLoc, areFacesEnabled);
  glUniform1f(wireframeLoc, isWireframeEnabled);
  glUniform4f(wireframeColourLoc, wireframeColour.r, wireframeColour.g,
                                  wireframeColour.b, wireframeColour.a);
  
  if (isCullingEnabled) {
    glEnable(GL_CULL_FACE);
  }
  glBindVertexArray(vao[Shader::FRACTAL]);
  glDrawElements(GL_TRIANGLES, fractal.indexCount, GL_UNSIGNED_INT, 0);
  if (isCullingEnabled) {
    glDisable(GL_CULL_FACE);
  }
  glBindVertexArray(0);

  if (areNormalsEnabled) {
    glUseProgram(normalShader);

    modelLoc      = glGetUniformLocation(normalShader, "model");
    viewLoc       = glGetUniformLocation(normalShader, "view");
    projectionLoc = glGetUniformLocation(normalShader, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(camera.view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,value_ptr(camera.projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

    normalLengthLoc = glGetUniformLocation(normalShader, "normalLength");
    glUniform1f(normalLengthLoc, defaultNormalLength *
                                 normalLength * scaleFactor);
    
    glBindVertexArray(vao[Shader::FRACTAL]);
    glDrawElements(GL_TRIANGLES, fractal.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
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
    glClearColor(backgroundColour.r, backgroundColour.g,
                 backgroundColour.b, 1.0f);
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
  Shader shader("src/shaders/fractal.vert", "src/shaders/fractal.frag",
                "src/shaders/fractal.geom");
  fractalShader = shader.programID;
  shader = Shader("src/shaders/fractal.vert", "src/shaders/normal.frag",
                  "src/shaders/normal.geom");
  normalShader = shader.programID;
}

/**
 * Generate the fractal and load the vertex/index data into the buffers.
 */
GLvoid updateFractalBuffer()
{
  GLfloat vertexBufferSize = fractal.vertexCount * fractal.DIMENSIONS *
                             fractal.attributeCount * sizeof(GLfloat);
  GLfloat indexBufferSize = fractal.indexCount * sizeof(GLuint);

  glBindVertexArray(vao[Shader::FRACTAL]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[Shader::FRACTAL]);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize,
               fractal.vertexData, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[Shader::FRACTAL]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize,
               fractal.indexData, GL_STATIC_DRAW);

  addVertexAttributes(fractalShader);

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
  const GLint attributeCount = 3;
  const GLchar* attributeNames[attributeCount] =
                {"position", "normal", "colour"};
  GLint attributeSizes[attributeCount] = {3, 3, 3};

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
  GLuint isFullscreen = env["isFullScreenEnabled"];

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
    monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
    width = videoMode->width;
    height = videoMode->height;
  }
  window = glfwCreateWindow(width, height, "Fractals", monitor, nullptr);

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set the clear colour value.
  backgroundColour.r = env["backgroundColourRed"];
  backgroundColour.g = env["backgroundColourGreen"];
  backgroundColour.b = env["backgroundColourBlue"];
}

/**
 * Terminate the application gracefully by deleting objects and properly
 * closing the window.
 */
GLvoid terminateGraphics()
{
  glDeleteProgram(fractalShader);

  for (GLuint i = Shader::FRACTAL; i != Shader::NONE; i++) {
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
  profile = (argc >= 2) ? argv[1] : "profile.txt";

  // Initialise the envorinment properties.
  initialiseEnvironment();

  // Initialise the graphics environment.
  initialiseGraphics(argc, argv);
  
  // Initialise the buffers and shaders.
  initialiseBuffersAndShaders();

  // Generate the fractal.
  generateFractal();

  // Push the vertex data into the buffers.
  updateFractalBuffer();

  // Run the graphics loop.
  runMainLoop();

  // Close the application gracefully.
  terminateGraphics();

  return 0;
}
