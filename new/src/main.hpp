/**
 * [Program description]
 */

// Statically link with GLEW.
#define GLEW_STATIC

// System headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include "helpers.hpp"
#include "camera.cpp"
#include "shader.cpp"
#include "fractal.cpp"

#define true  1
#define false 0
#define DEFAULT_WINDOW_WIDTH  1200
#define DEFAULT_WINDOW_HEIGHT 675

GLvoid initialiseAll();
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode);
GLvoid mouse(GLFWwindow* window, GLdouble x, GLdouble y);
GLvoid updateTime();
GLvoid initialiseEnvironment();
GLvoid initialiseCamera();
GLvoid updateCamera();
GLvoid initialiseFractal();
GLvoid generateFractal();
GLvoid drawFractal();
GLvoid runMainLoop();
GLvoid initialiseBuffersAndShaders();
GLvoid updateFractalBuffer();
GLvoid addVertexAttributes(GLuint shaderID);
GLvoid initialiseGraphics(GLint argc, GLchar* argv[]);
GLvoid terminateGraphics();
GLint main(GLint argc, GLchar* argv[]);