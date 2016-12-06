// Statically link with GLEW.
#define GLEW_STATIC

// System headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "shader.cpp"
#include "camera.cpp"

#define true  1
#define false 0
#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 675

// function prototypes
GLfloat randomNumber(GLfloat min, GLfloat max);
GLfloat average(std::initializer_list<GLfloat> values);
GLvoid move();
GLvoid keyboard(GLFWwindow* window, GLint key, GLint scancode,
                GLint action, GLint mode);
GLvoid mouse(GLFWwindow* window, GLdouble x, GLdouble y);
GLvoid updateTime();
GLvoid runMainLoop();
GLvoid initialiseGraphics(GLint argc, GLchar* argv[]);
GLvoid terminateGraphics();
GLint main(GLint argc, GLchar* argv[]);

class Fractal
{
  public:
    // vertices contain x, y, z, r, g, b (positions, colours)
    GLfloat* vertexData;
    // vertex count
    GLuint vertexCount;
    // indices tell opengl how to draw the vertices
    GLuint* indexData;
    // index count
    GLuint indexCount;

    // y values of each vertex
    GLfloat* rawYValues;
    // number of iterations in the algorithm
    GLuint depth;
    // width/height of the fractal
    GLuint size;
    // y value range
    GLfloat yRange;
    // y range increment
    GLfloat yRangeIncrement;
    // y value deviance per iteration
    GLfloat yDeviance;
    // y deviance increment
    GLfloat yDevianceIncrement;
    // average of all y values
    GLfloat averageYValue;

    Fractal(GLuint depth, GLfloat yRange, GLfloat deviance);
    GLvoid  setY(GLuint x, GLuint z, GLfloat value);
    GLfloat getY(GLuint x, GLuint z);
    GLvoid generate();
};