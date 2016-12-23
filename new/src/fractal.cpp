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

  positions = std::vector<std::vector<glm::vec3>>(size,
                          std::vector<glm::vec3>(size));
  normals   = std::vector<std::vector<glm::vec3>>(size,
                          std::vector<glm::vec3>(size));
  colours   = std::vector<std::vector<glm::vec3>>(size,
                          std::vector<glm::vec3>(size));

  GLuint vertexDataSize = vertexCount * DIMENSIONS;
  indexData  = new GLuint[indexCount];
  vertexData = new GLfloat[vertexDataSize * attributeCount];
  normalVertexData = new GLfloat[vertexDataSize * 2];
}

/**
 * Set the Y value at a given vertex in the 
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  positions[x % size][z % size].y = value;
}

/**
 * Get the Y value at a given vertex in the 
 */
GLfloat Fractal::getY(GLuint x, GLuint z)
{
  return positions[x % size][z % size].y;
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
  smoothNormals(createBoxKernel(3));

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
  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      positions[i][j].x = (GLfloat)(i % (GLuint)size) / (GLfloat)size;
      // The Y values are already set within generate().
      positions[i][j].z = (GLfloat)j / (GLfloat)size;
    }
  }
}

/**
 * Update the normal of each vertex.
 */
GLvoid Fractal::updateNormals()
{
  glm::vec3 p1, p2, p3, v1, v2, normal;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      p1 = positions[i][j];
      p2 = positions[i][(j + 1) % size];
      p3 = positions[(i + 1) % size][j];

      v1 = p2 - p1,
      v2 = p3 - p1;

      normal.x = (v1.y * v2.z) - (v1.z * v2.y);
      normal.y = (v1.z * v2.x) - (v1.x * v2.z);
      normal.z = (v1.x * v2.y) - (v1.y * v2.x);
      normals[i][j] = glm::normalize(normal);
    }
  }
}

/**
 * Update the colour of each vertex. Noise (greyscale) can be added to vary the
 * vertex colours.
 */
GLvoid Fractal::updateColours(GLfloat noise)
{
  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      GLfloat randomNoise = randomNumber(-noise, noise);

      colours[i][j].x = baseColour.x + randomNoise;
      colours[i][j].y = baseColour.y + randomNoise;
      colours[i][j].z = baseColour.z + randomNoise;
    }
  }
}

/**
 * Generate the vertex data.
 */
GLvoid Fractal::generateVertexData()
{
  GLuint offset = 0;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      vertexData[offset++] = positions[i][j].x;
      vertexData[offset++] = positions[i][j].y;
      vertexData[offset++] = positions[i][j].z;

      vertexData[offset++] = normals[i][j].x;
      vertexData[offset++] = normals[i][j].y;
      vertexData[offset++] = normals[i][j].z;

      vertexData[offset++] = colours[i][j].x;
      vertexData[offset++] = colours[i][j].y;
      vertexData[offset++] = colours[i][j].z;
    }
  }
}

/**
 * Generate the normal vertex data.
 */
GLvoid Fractal::generateNormalVertexData()
{
  GLuint offset = 0;
  GLfloat normalLength = 1.0f / size;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      normalVertexData[offset++] = positions[i][j].x;
      normalVertexData[offset++] = positions[i][j].y;
      normalVertexData[offset++] = positions[i][j].z;

      normalVertexData[offset++] = positions[i][j].x +
                                   (normals[i][j].x * normalLength);
      normalVertexData[offset++] = positions[i][j].y +
                                   (normals[i][j].y * normalLength);
      normalVertexData[offset++] = positions[i][j].z +
                                   (normals[i][j].z * normalLength);
    }
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
  GLuint kernelSize = kernel.size();
  glm::vec3 accumulator;
  std::vector<std::vector<glm::vec3>> newNormals(size,
                          std::vector<glm::vec3>(size));

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      accumulator = glm::vec3(0.0f);

      for (GLuint k = 0; k < kernelSize; k++) {
        for (GLuint l = 0; l < kernelSize; l++) {
          accumulator += normals[(i + (k - (kernelSize / 2))) % size]
                                [(j + (l - (kernelSize / 2))) % size] *
                         kernel[k][l];
        }
      }
      newNormals[i][j] = accumulator;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      normals[i][j] = newNormals[i][j];
    }
  }
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
      GLfloat weight = exp(-0.5f *
                       (pow(((GLfloat)i - halfSize) / sigma, 2.0f) +
                        pow(((GLfloat)j - halfSize) / sigma, 2.0f))) /
                       denominator;

      accumulator += weight;
      kernel[i][j] = weight;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      kernel[i][j] /= accumulator;
    }
  }

  return kernel;
}

/**
 * Create a Box filter kernel of a given size.
 */
std::vector<std::vector<GLfloat>> Fractal::createBoxKernel(GLuint size)
{
  std::vector<std::vector<GLfloat>> kernel(size, std::vector<GLfloat>(size));
  GLfloat weight = 1.0f / (GLfloat)(size * size);

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      kernel[i][j] = weight;
    }
  }

  return kernel;
}