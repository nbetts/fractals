/**
 * [Program description]
 */

#include "fractal.hpp"
#include "helpers.hpp"

/**
 * Constructor to initialise the fractal with the given properties.
 */
Fractal::Fractal(GLuint desiredDepth, GLfloat desiredYRange,
                 GLfloat desiredYDeviance, glm::vec3 desiredBaseColour)
{
  depth = desiredDepth;
  size = 2 << (depth - 1);
  yRange = desiredYRange;
  yDeviance = desiredYDeviance;
  baseColour = desiredBaseColour;

  indexCount = (size - 1) * (size - 1) * (2 * DIMENSIONS);
  vertexCount = size * size;
  attributeCount = 3;

  positions = new GLfloat[vertexCount * DIMENSIONS];
  normals   = new GLfloat[vertexCount * DIMENSIONS];
  colours   = new GLfloat[vertexCount * DIMENSIONS];

  rawYValues = new GLfloat[vertexCount];
  indexData  = new GLuint[indexCount];
  vertexData = new GLfloat[vertexCount * DIMENSIONS * attributeCount];
  normalVertexData = new GLfloat[vertexCount * DIMENSIONS * 2];
}

/**
 * Set the Y value at a given vertex in the 
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) * size;

  rawYValues[position] = value;
  positions[position * DIMENSIONS + 1] = value;
  vertexData[(position * DIMENSIONS * attributeCount) + 1] = value;
}

/**
 * Get the Y value at a given vertex in the 
 */
GLfloat Fractal::getY(GLuint x, GLuint z)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) * size;

  return rawYValues[position];
}

/**
 * Recursively update the points in the fractal using the midpoint displacement
 * algorithm.
 */
GLvoid Fractal::generate()
{
  GLuint tempSize = size;
  GLfloat tempYRange = yRange;

  // Generate the Y values of each vertex.
  while (tempSize > 1) {
    GLuint halfStep = tempSize / 2;

    for (GLuint y = halfStep; y < size + halfStep; y += tempSize) {
      for (GLuint x = halfStep; x < size + halfStep; x += tempSize) {
        GLuint hs = tempSize / 2;
        GLfloat a = getY(x - hs, y - hs);
        GLfloat b = getY(x + hs, y - hs);
        GLfloat c = getY(x - hs, y + hs);
        GLfloat d = getY(x + hs, y + hs);
        setY(x, y, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
      }
    }

    for (GLuint y = 0; y < size; y += tempSize) {
      for (GLuint x = 0; x < size; x += tempSize) {
        GLuint hs = tempSize / 2;
        GLfloat newX = x + halfStep;
        GLfloat a = getY(newX - hs, y);
        GLfloat b = getY(newX + hs, y);
        GLfloat c = getY(newX, y - hs);
        GLfloat d = getY(newX, y + hs);
        setY(newX, y, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
   
        GLfloat newY = y + halfStep;
        a = getY(x - hs, newY);
        b = getY(x + hs, newY);
        c = getY(x, newY - hs);
        d = getY(x, newY + hs);
        setY(x, newY, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
      }
    }

    tempSize /= 2;
    tempYRange *= yDeviance;
  }

  updatePositions();
  std::vector<std::vector<GLfloat>> kernel = {{0.0625f, 0.125f, 0.0625f},
                                              { 0.125f, 0.25f,  0.125f},
                                              {0.0625f, 0.125f, 0.0625f}};
  smoothPositions(kernel);

  updateNormals();
  updateColours();

  generateVertexData();
  generateNormalVertexData();
  generateIndexData();
}

/**
 * Update the colour of each vertex.
 */
GLvoid Fractal::updatePositions()
{
  GLuint offset = 0;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      positions[offset++] = (GLfloat)(i % (GLuint)size) / (GLfloat)size;
      positions[offset++] = getY(i, j);
      positions[offset++] = (GLfloat)j / (GLfloat)size;

      // printf("v %.2f %.2f %.2f\n", positions[offset - 3],
      //                              positions[offset - 2],
      //                              positions[offset - 1]);
    }
  }
}

/**
 * Update the normal of each vertex.
 */
GLvoid Fractal::updateNormals()
{
  GLuint offset = 0, increment = 0;
  glm::vec3 p1, p2, p3, v1, v2, normal;

  for (GLuint i = 0; i < vertexCount; i++) {
    p1 = glm::vec3(positions[(increment + 0) % (vertexCount * DIMENSIONS)],
                   positions[(increment + 1) % (vertexCount * DIMENSIONS)],
                   positions[(increment + 2) % (vertexCount * DIMENSIONS)]);
    p2 = glm::vec3(positions[(increment + 3) % (vertexCount * DIMENSIONS)],
                   positions[(increment + 4) % (vertexCount * DIMENSIONS)],
                   positions[(increment + 5) % (vertexCount * DIMENSIONS)]);
    p3 = glm::vec3(positions[((size * DIMENSIONS) + increment + 0) %
                             (vertexCount * DIMENSIONS)],
                   positions[((size * DIMENSIONS) + increment + 1) %
                             (vertexCount * DIMENSIONS)],
                   positions[((size * DIMENSIONS) + increment + 2) %
                             (vertexCount * DIMENSIONS)]);

    v1 = p2 - p1,
    v2 = p3 - p1;

    // printf("\n");
    // printf("p1: %.2f %.2f %.2f\n", p1.x, p1.y, p1.z);
    // printf("p2: %.2f %.2f %.2f\n", p2.x, p2.y, p2.z);
    // printf("p3: %.2f %.2f %.2f\n", p3.x, p3.y, p3.z);
    // printf("v1: %.2f %.2f %.2f\n", v1.x, v1.y, v1.z);
    // printf("v2: %.2f %.2f %.2f\n", v2.x, v2.y, v2.z);

    normal.x = (v1.y * v2.z) - (v1.z * v2.y);
    normal.y = (v1.z * v2.x) - (v1.x * v2.z);
    normal.z = (v1.x * v2.y) - (v1.y * v2.x);
    // normal = glm::cross(v1, v2);
    normal = glm::normalize(normal);

    normals[offset++] = normal.x;
    normals[offset++] = normal.y;
    normals[offset++] = normal.z;

    increment += DIMENSIONS;
  }
}

/**
 * Update the colour of each vertex.
 */
GLvoid Fractal::updateColours()
{
  GLuint offset = 0;
  GLfloat yValue;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      yValue = getY(i, j);

      colours[offset++] = baseColour.x;
      colours[offset++] = baseColour.y;
      colours[offset++] = baseColour.z;
    }
  }
}

/**
 * Generate the vertex data.
 */
GLvoid Fractal::generateVertexData()
{
  GLuint offset = 0, increment = 0;

  for (GLuint i = 0; i < vertexCount; i++) {
    vertexData[offset++] = positions[increment + 0];
    vertexData[offset++] = positions[increment + 1];
    vertexData[offset++] = positions[increment + 2];

    vertexData[offset++] = normals[increment + 0];
    vertexData[offset++] = normals[increment + 1];
    vertexData[offset++] = normals[increment + 2];

    vertexData[offset++] = colours[increment + 0];
    vertexData[offset++] = colours[increment + 1];
    vertexData[offset++] = colours[increment + 2];

    increment += DIMENSIONS;

    // printf("v %.2f %.2f %.2f   n %.2f %.2f %.2f   c %.2f %.2f %.2f\n",
    //          vertexData[offset - 9],
    //          vertexData[offset - 8],
    //          vertexData[offset - 7],
    //          vertexData[offset - 6],
    //          vertexData[offset - 5],
    //          vertexData[offset - 4],
    //          vertexData[offset - 3],
    //          vertexData[offset - 2],
    //          vertexData[offset - 1]);
  }
}

/**
 * Generate the normal vertex data.
 */
GLvoid Fractal::generateNormalVertexData()
{
  GLuint offset = 0, increment = 0;

  for (GLuint i = 0; i < vertexCount; i++) {
    normalVertexData[offset++] = positions[increment + 0];
    normalVertexData[offset++] = positions[increment + 1];
    normalVertexData[offset++] = positions[increment + 2];

    normalVertexData[offset++] = positions[increment + 0] +
                                 (normals[increment + 0] * 0.01f);
    normalVertexData[offset++] = positions[increment + 1] +
                                 (normals[increment + 1] * 0.01f);
    normalVertexData[offset++] = positions[increment + 2] +
                                 (normals[increment + 2] * 0.01f);

    // printf("v %.2f %.2f %.2f\tn %.2f %.2f %.2f\tnvd %.2f %.2f %.2f\n",
    //        positions[increment + 0],
    //        positions[increment + 1],
    //        positions[increment + 2],
    //        normals[increment + 0],
    //        normals[increment + 1],
    //        normals[increment + 2],
    //        normalVertexData[offset - 3],
    //        normalVertexData[offset - 2],
    //        normalVertexData[offset - 1]);

    increment += DIMENSIONS;
  }
}

/**
 * Generate the index data.
 */
GLvoid Fractal::generateIndexData()
{
  GLuint offset = 0, increment = 0;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size - 1; j++) {
      if (increment != 0 && (increment + 1) % size == 0) {
        increment++;
        continue;
      }

      indexData[offset++] = increment;
      indexData[offset++] = increment + 1;
      indexData[offset++] = increment + size;

      indexData[offset++] = increment + size;
      indexData[offset++] = increment + 1;
      indexData[offset++] = increment + size + 1;

      increment++;
    }
  }
}

/**
 * Perform a convolution with a given kernel to smooth the fractal vertices.
 */
GLvoid Fractal::smoothPositions(std::vector<std::vector<GLfloat>> kernel)
{
  GLuint kernelSize = kernel.size();
  GLfloat accumulator;
  GLfloat newYValues[size][size];

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      accumulator = 0.0f;

      for (GLuint k = 0; k < kernelSize; k++) {
        for (GLuint l = 0; l < kernelSize; l++) {
          accumulator += getY(i + (k - (kernelSize / 2)),
                              j + (l - (kernelSize / 2))) *
                         kernel[k][l];
        }
      }
      newYValues[i][j] = accumulator;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      setY(i, j, newYValues[i][j]);
    }
  }
}

/**
 * Perform a convolution with a given kernel to smooth the fractal normals.
 */
GLvoid Fractal::smoothNormals(std::vector<std::vector<GLfloat>> kernel)
{
  // TODO
}

/**
 * Perform a convolution with a given kernel to smooth the fractal colours.
 */
GLvoid Fractal::smoothColours(std::vector<std::vector<GLfloat>> kernel)
{
  // TODO
}
