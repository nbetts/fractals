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
  GLuint vertexDataSize = vertexCount * DIMENSIONS;

  positions = new GLfloat[vertexDataSize];
  normals   = new GLfloat[vertexDataSize];
  colours   = new GLfloat[vertexDataSize];

  indexData  = new GLuint[indexCount];
  vertexData = new GLfloat[vertexDataSize * attributeCount];
  normalVertexData = new GLfloat[vertexDataSize * 2];
}

/**
 * Set the Y value at a given vertex in the 
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) * size;

  positions[position * DIMENSIONS + 1] = value;
}

/**
 * Get the Y value at a given vertex in the 
 */
GLfloat Fractal::getY(GLuint x, GLuint z)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) * size;

  return positions[position * DIMENSIONS + 1];
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
  smoothPositions(createGaussianKernel(3, 5.0f));

  updateNormals();
  updateColours(0.015f);

  generateVertexData();
  generateNormalVertexData();
  generateIndexData();
}

/**
 * Update the position of each vertex.
 */
GLvoid Fractal::updatePositions()
{
  GLuint offset = 0;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      positions[offset++] = (GLfloat)(i % (GLuint)size) / (GLfloat)size;
      offset++; // Y values are already set in generate().
      positions[offset++] = (GLfloat)j / (GLfloat)size;
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
    p1 = glm::vec3(positions[(increment + 0)],
                   positions[(increment + 1)],
                   positions[(increment + 2)]);
    p2 = glm::vec3(positions[(increment + 3)],
                   positions[(increment + 4)],
                   positions[(increment + 5)]);
    p3 = glm::vec3(positions[((size * DIMENSIONS) + increment + 0)],
                   positions[((size * DIMENSIONS) + increment + 1)],
                   positions[((size * DIMENSIONS) + increment + 2)]);

    v1 = p2 - p1,
    v2 = p3 - p1;

    normal.x = (v1.y * v2.z) - (v1.z * v2.y);
    normal.y = (v1.z * v2.x) - (v1.x * v2.z);
    normal.z = (v1.x * v2.y) - (v1.y * v2.x);
    normal = glm::normalize(normal);

    normals[offset++] = normal.x;
    normals[offset++] = normal.y;
    normals[offset++] = normal.z;

    increment += DIMENSIONS;
  }
}

/**
 * Update the colour of each vertex. Noise (greyscale) can be added to vary the
 * vertex colours.
 */
GLvoid Fractal::updateColours(GLfloat noise)
{
  GLuint offset = 0;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      GLfloat randomNoise = randomNumber(-noise, noise);

      colours[offset++] = baseColour.x + randomNoise;
      colours[offset++] = baseColour.y + randomNoise;
      colours[offset++] = baseColour.z + randomNoise;
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
  }
}

/**
 * Generate the normal vertex data.
 */
GLvoid Fractal::generateNormalVertexData()
{
  GLuint offset = 0, increment = 0;
  GLfloat normalLength = 1.0f / size;

  for (GLuint i = 0; i < vertexCount; i++) {
    normalVertexData[offset++] = positions[increment + 0];
    normalVertexData[offset++] = positions[increment + 1];
    normalVertexData[offset++] = positions[increment + 2];

    normalVertexData[offset++] = positions[increment + 0] +
                                 (normals[increment + 0] * normalLength);
    normalVertexData[offset++] = positions[increment + 1] +
                                 (normals[increment + 1] * normalLength);
    normalVertexData[offset++] = positions[increment + 2] +
                                 (normals[increment + 2] * normalLength);

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

/**
 * Create a Gaussian filter kernel of a given size and sigma value.
 */
std::vector<std::vector<GLfloat>> Fractal::createGaussianKernel(GLuint size,
                                                                GLfloat sigma)
{
  std::vector<std::vector<GLfloat>> kernel(size, std::vector<GLfloat>(size));

  GLuint halfSize = size / 2;
  GLfloat denominator = 2.0f * M_PI * sigma * sigma;
  GLfloat accumulator = 0.0f;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      GLfloat val = exp(-0.5f * (pow(((GLfloat)i - halfSize) / sigma, 2.0f) +
                                 pow(((GLfloat)j - halfSize) / sigma, 2.0f))) /
                    denominator;

      accumulator += val;
      kernel[i][j] = val;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      kernel[i][j] /= accumulator;
    }
  }

  return kernel;
}