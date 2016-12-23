/**
 * [Program description]
 */

#ifndef FRACTAL_HEADER
#define FRACTAL_HEADER

class Fractal
{
  public:
    static const GLuint DIMENSIONS = 3;

    /**
     * depth - number of iterations in the diamond-square algorithm
     * size - width/height of the fractal
     * yRange - Y value range per iteration
     * yRangeIncrement - Y value increment
     * yDeviance - (+/-) Y value deviance per iteration
     * yDevianceIncrement - Y deviance increment
     * baseColour - base colour of the fractal
     *
     * indexCount - number of indices for drawing the fractal
     * vertexCount - number of vertices
     * attributeCount - number of vertex attributes
     *
     * positions - array representing positions of vertices
     * normals - array representing normals of vertices
     * colours - array representing final colours of vertices
     *
     * indexData - index array representing triplets of vertices
     * vertexData - combined data as [positions, normals, colours]
     * normalVertexData - similar to vertexData but for normals
     */
    GLuint depth;
    GLuint size;
    GLfloat yRange;
    GLfloat yRangeIncrement;
    GLfloat yDeviance;
    GLfloat yDevianceIncrement;
    glm::vec3 baseColour;

    GLuint indexCount;
    GLuint vertexCount;
    GLuint attributeCount;

    std::vector<std::vector<glm::vec3>> positions;
    std::vector<std::vector<glm::vec3>> normals;
    std::vector<std::vector<glm::vec3>> colours;

    GLuint* indexData;
    GLfloat* vertexData;
    GLfloat* normalVertexData;

    Fractal(GLuint desiredDepth, GLfloat desiredYRange,
            GLfloat desiredYDeviance, glm::vec3 desiredBaseColour);
    GLvoid  setYPosition(GLuint x, GLuint z, GLfloat value);
    GLfloat getYPosition(GLuint x, GLuint z);
    GLvoid generate();
    GLvoid generateIndexData();
    GLvoid generateVertexData();
    GLvoid generateNormalVertexData();
    GLvoid updateVertexData();
    GLvoid updatePositions();
    GLvoid updateNormals();
    GLvoid updateColours();
    GLvoid smoothPositions(std::vector<std::vector<GLfloat>> kernel);
    GLvoid smoothNormals(std::vector<std::vector<GLfloat>> kernel);
    GLvoid smoothColours(std::vector<std::vector<GLfloat>> kernel);
    std::vector<std::vector<GLfloat>> createGaussianKernel(GLuint size,
                                                           GLfloat sigma);
    std::vector<std::vector<GLfloat>> createBoxKernel(GLuint size);
    GLvoid addColourNoise(GLfloat noiseLevel);
};

#endif
