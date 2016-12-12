/**
 * [Program description]
 */

#ifndef FRACTAL_HEADER
#define FRACTAL_HEADER

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
    // number of iterations in the subdivision algorithm
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
    // colour
    glm::vec3 colour;

    Fractal(GLuint depth, GLfloat yRange, GLfloat deviance, glm::vec3 dColour);
    GLvoid  setY(GLuint x, GLuint z, GLfloat value);
    GLfloat getY(GLuint x, GLuint z);
    GLvoid generate();
    GLvoid convolve(GLuint kernelSize, GLfloat** kernel);
};

#endif
