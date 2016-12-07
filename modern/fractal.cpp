#include "fractal.hpp"

GLFWwindow* window;
GLint frameWidth, frameHeight;
GLuint fractalVao, vbo, ebo;
GLuint fractalShaderID;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f),
              glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.0f, 0.2f, 45.0f);
Fractal fractal(6, 0.2f, 0.5f);

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

// Vertex attributes
const GLint attributeCount = 2;
const GLchar* attributeNames[attributeCount] = {"position", "color"};
GLint attributeSizes[attributeCount] = {3, 3};

/**
 * Constructor to initialise the fractal with the given properties.
 */
Fractal::Fractal(GLuint desiredDepth, GLfloat desiredYRange,
                 GLfloat desiredYDeviance)
{
  depth = desiredDepth;
  size = pow(2, depth);
  yRange = desiredYRange;
  yRangeIncrement = 0.02f;
  yDeviance = desiredYDeviance;
  yDevianceIncrement = 0.02f;

  vertexCount = pow(size, 2);
  vertexData = new GLfloat[vertexCount * 6];
  rawYValues = new GLfloat[vertexCount];

  indexCount = pow(size - 1, 2) * 6;
  indexData = new GLuint[indexCount];
}

/**
 * Set the Y value at a given vertex in the fractal.
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  GLuint position = (x & (fractal.size - 1)) + (z & (fractal.size - 1)) *
               fractal.size;

  fractal.rawYValues[position] = value;
  fractal.averageYValue += value;
}

/**
 * Get the Y value at a given vertex in the fractal.
 */
GLfloat Fractal::getY(GLuint x, GLuint z)
{
  GLuint position = (x & (fractal.size - 1)) + (z & (fractal.size - 1)) *
               fractal.size;

  return fractal.rawYValues[position];
}

/**
 * Return a random real number in the given range.
 */
GLfloat randomNumber(GLfloat min, GLfloat max)
{
  return min + rand() / (RAND_MAX / (max - min) + 1.0f);
}

/**
 * Calculate the average value of a list of values.
 */
GLfloat average(std::initializer_list<GLfloat> values)
{
  GLfloat sum = 0.0f;
  GLuint count = 0;

  for(auto value : values) {
    sum += value;
    count++;
  }

  return sum / (GLfloat)count;
}

/**
 * Recursively update the points in the fractal using the midpoint displacement
 * algorithm.
 */
GLvoid Fractal::generate()
{
  fractal.averageYValue = 0.0f;
  GLuint fractalSize = fractal.size;
  GLfloat yRange = fractal.yRange;

  // Generate the Y values of each vertex.
  while (fractalSize > 1) {
    GLuint halfStep = fractalSize / 2;

    for (GLuint y = halfStep; y < fractal.size + halfStep; y += fractalSize) {
      for (GLuint x = halfStep; x < fractal.size + halfStep; x += fractalSize) {
        GLuint hs = fractalSize / 2;
        GLfloat a = getY(x - hs, y - hs);
        GLfloat b = getY(x + hs, y - hs);
        GLfloat c = getY(x - hs, y + hs);
        GLfloat d = getY(x + hs, y + hs);
        setY(x, y, average({a, b, c, d}) + randomNumber(-yRange, yRange));
      }
    }

    for (GLuint y = 0; y < fractal.size; y += fractalSize) {
      for (GLuint x = 0; x < fractal.size; x += fractalSize) {
        GLuint hs = fractalSize / 2;
        GLfloat newX = x + halfStep;
        GLfloat a = getY(newX - hs, y);
        GLfloat b = getY(newX + hs, y);
        GLfloat c = getY(newX, y - hs);
        GLfloat d = getY(newX, y + hs);
        setY(newX, y, average({a, b, c, d}) + randomNumber(-yRange, yRange));
   
        GLfloat newY = y + halfStep;
        a = getY(x - hs, newY);
        b = getY(x + hs, newY);
        c = getY(x, newY - hs);
        d = getY(x, newY + hs);
        setY(x, newY, average({a, b, c, d}) + randomNumber(-yRange, yRange));
      }
    }

    fractalSize /= 2;
    yRange *= fractal.yDeviance;
  }

  // Generate the X, Z and color values of each vertex.
  GLuint i, j, counter = 0;
  GLfloat yDifference = fractal.yRange * 2.0f;

  for (i = 0; i < fractal.size; i++) {
    for (j = 0; j < fractal.size; j++) {
      GLfloat yValue = getY(i, j);
      fractal.vertexData[counter++] = (GLfloat)(i % (GLuint)fractal.size) /
                                      (GLfloat)fractal.size;
      fractal.vertexData[counter++] = yValue;
      fractal.vertexData[counter++] = (GLfloat)j / (GLfloat)fractal.size;
      fractal.vertexData[counter++] = 0.0f;
      fractal.vertexData[counter++] = 1.0f;
      fractal.vertexData[counter++] = yValue / yDifference + 0.5f;
    }
  }

  // Generate the indices.
  counter = 0;
  GLuint increment = 0;

  for (i = 0; i <= fractal.indexCount + 6; i += 6) {
    if (increment != 0 && (increment + 1) % fractal.size == 0) {
      increment++;
      continue;
    }

    fractal.indexData[counter++] = increment;
    fractal.indexData[counter++] = increment + 1;
    fractal.indexData[counter++] = increment + fractal.size;

    fractal.indexData[counter++] = increment + fractal.size;
    fractal.indexData[counter++] = increment + 1;
    fractal.indexData[counter++] = increment + 1 + fractal.size;

    increment++;
  }

  // reset the camera's focal point.
  // camera.focus.y = 0;
}

/**
 * Generate the fractal and reload the vertex/index data into the vbo and ebo.
 */
GLvoid reloadFractal()
{
  fractal.generate();

  // Bind the vao and the  vertices to the vbo.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, fractal.vertexCount * 6 * 4,
               fractal.vertexData, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, fractal.indexCount * 4,
               fractal.indexData, GL_STATIC_DRAW);

  // Add layout attributes to the shader.
  GLint i, stride = 0, offset = 0;

  for (i = 0; i < attributeCount; i++) {
    stride += attributeSizes[i];
  }

  for (i = 0; i < attributeCount; i++) {
    GLint attribute = glGetAttribLocation(fractalShaderID,
                                          attributeNames[i]);
    glVertexAttribPointer(attribute, attributeSizes[i], GL_FLOAT,
                          GL_FALSE, stride * sizeof(GLfloat),
                          (GLvoid*)(offset * sizeof(GLfloat)));
    glEnableVertexAttribArray(attribute);
    offset += attributeSizes[i];
  }

  // Unbind the vertex buffer and array objects.
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * Update the camera's position relative to the world.
 */
GLvoid move()
{
  camera.setMovementSpeed(deltaTime);
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

  // printf("%.2f %.2f %.2f\n", camera.position.x,
  //        camera.position.y, camera.position.z);
}

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

  /**
   * If set to true, regenerate the fractal.
   */
  GLuint regenerate = false;

  switch(key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GL_TRUE);
      break;
    case GLFW_KEY_SPACE:
      regenerate = true;
      break;
    case GLFW_KEY_K:
      if (fractal.yRange - fractal.yRangeIncrement >= 0) {
        fractal.yRange -= fractal.yRangeIncrement;
        regenerate = true;
      }
      break;
    case GLFW_KEY_I:
      fractal.yRange += fractal.yRangeIncrement;
      regenerate = true;
      break;
    case GLFW_KEY_J:
      if (fractal.yDeviance - fractal.yDevianceIncrement >= 0) {
        fractal.yDeviance -= fractal.yDevianceIncrement;
        regenerate = true;
      }
      break;
    case GLFW_KEY_L:
      fractal.yDeviance += fractal.yDevianceIncrement;
      regenerate = true;
      break;
    case GLFW_KEY_U:
      if (fractal.depth - 1 > 0) {
        fractal.depth--;
        regenerate = true;
      }
      break;
    case GLFW_KEY_O:
      fractal.depth++;
      regenerate = true;
      break;
  }

  if (regenerate) {
    fractal = Fractal(fractal.depth, fractal.yRange, fractal.yDeviance);
    reloadFractal();
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
                             0.01f, 100.0f);

    // Update the camera view matrix.
    move();
    view = camera.getLookAtMatrix();

    // Clear the screen.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the current shader program.
    glUseProgram(fractalShaderID);
    GLuint lightColorLoc = glGetUniformLocation(fractalShaderID, "lightColor");
    glUniform3f(lightColorLoc, 0.2f, 1.0f, 1.0f);

    // Transform the shader program's vertices with the model, view and
    // projection matrices.
    GLuint modelLoc = glGetUniformLocation(fractalShaderID, "model");
    GLuint viewLoc = glGetUniformLocation(fractalShaderID, "view");
    GLuint projectionLoc = glGetUniformLocation(fractalShaderID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

    // Draw functions.
    glBindVertexArray(fractalVao);
    mat4 model;
    model = translate(model, vec3(0.0f, 0.4f, -0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

    // glDrawArrays(GL_TRIANGLES, 0, fractal.vertexCount);
    glDrawElements(GL_TRIANGLES, fractal.indexCount, GL_UNSIGNED_INT, 0);
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

  if (isFullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(videoMode->width, videoMode->width,
                              "Diamond-Square Fractal", monitor,
                              NULL);
  } else {
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                              "Diamond-Square Fractal", NULL, NULL);
  }

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
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


/**
 * Terminate the application gracefully by deleting objects and properly
 * closing the window.
 */
GLvoid terminateGraphics()
{
  glDeleteProgram(fractalShaderID);
  glDeleteVertexArrays(1, &fractalVao);
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glfwTerminate();
}

/**
 * Main method.
 */
GLint main(GLint argc, GLchar* argv[])
{
  srand(time(NULL));

  // Reset the camera positions to ensure they are correct.
  camera.position = glm::vec3(0.5f, 0.5f, 1.0f);
  camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
  camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Initialise the graphics environment.
  initialiseGraphics(argc, argv);
  
  // Create the vertex array and buffer objects.
  glGenVertexArrays(1, &fractalVao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(fractalVao);

  // Load the vertex and fragment shaders into a shader program.
  Shader fractalShader("shaders/fractal.vert", "shaders/fractal.frag");
  fractalShaderID = fractalShader.getProgramID();

  // Create the initial fractal.
  reloadFractal();

  // Run the graphics loop.
  runMainLoop();

  // Close the application gracefully.
  terminateGraphics();

  return 0;
}
