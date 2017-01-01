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
GLvoid Fractal::setYPosition(GLuint x, GLuint z, GLfloat value)
{
  positions[x % size][z % size].y = value;
}

/**
 * Get the Y value at a given vertex in the 
 */
GLfloat Fractal::getYPosition(GLuint x, GLuint z)
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
        GLfloat a = getYPosition(x - hs, y - hs);
        GLfloat b = getYPosition(x + hs, y - hs);
        GLfloat c = getYPosition(x - hs, y + hs);
        GLfloat d = getYPosition(x + hs, y + hs);
        setYPosition(x, y, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
      }
    }

    for (GLuint y = 0; y < size; y += tempSize) {
      for (GLuint x = 0; x < size; x += tempSize) {
        GLuint hs = tempSize / 2;
        GLfloat newX = x + halfStep;
        GLfloat a = getYPosition(newX - hs, y);
        GLfloat b = getYPosition(newX + hs, y);
        GLfloat c = getYPosition(newX, y - hs);
        GLfloat d = getYPosition(newX, y + hs);
        setYPosition(newX, y, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
   
        GLfloat newY = y + halfStep;
        a = getYPosition(x - hs, newY);
        b = getYPosition(x + hs, newY);
        c = getYPosition(x, newY - hs);
        d = getYPosition(x, newY + hs);
        setYPosition(x, newY, average({a, b, c, d}) +
             randomNumber(-tempYRange, tempYRange));
      }
    }

    tempSize /= 2;
    tempYRange *= yDeviance;
  }

  updatePositions();
  updateNormals();
  updateColours();
  updateVertexData();
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
  glm::vec3 p1, p2, p3, v1, v2;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      p1 = positions[i][j];
      p2 = positions[i][(j + 1) % size];
      p3 = positions[(i + 1) % size][j];

      // Account for edge cases.
      v1 = (i + 1 == size) ? p1 - p2 : p2 - p1;
      v2 = (j + 1 == size) ? p1 - p3 : p3 - p1;
      
      normals[i][j] = normalize(cross(v1, v2));
    }
  }
}

/**
 * Update the colour of each vertex.
 */
GLvoid Fractal::updateColours()
{
  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      colours[i][j].r = baseColour.r;
      colours[i][j].g = baseColour.g;
      colours[i][j].b = baseColour.b;
    }
  }
}

/**
 * Generate the vertex positional data.
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

      vertexData[offset++] = colours[i][j].r;
      vertexData[offset++] = colours[i][j].g;
      vertexData[offset++] = colours[i][j].b;
    }
  }
}

/**
 * Generate the vertex index data.
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
 * Generate all the vertex data.
 */
GLvoid Fractal::updateVertexData()
{
  generateVertexData();
  // generateNormalVertexData();
  generateIndexData();
}

/**
 * Perform a convolution with a given kernel to smooth the fractal vertices.
 */
GLvoid Fractal::smoothPositions(std::vector<std::vector<GLfloat>> kernel)
{
  GLuint kernelSize = kernel.size();
// printf("hi %d\n", size * size);
  GLfloat accumulator;
  GLfloat newYValues[size][size];

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      accumulator = 0.0f;

      for (GLuint k = 0; k < kernelSize; k++) {
        for (GLuint l = 0; l < kernelSize; l++) {
          accumulator += getYPosition(i + (k - (kernelSize / 2)),
                              j + (l - (kernelSize / 2))) *
                         kernel[k][l];
        }
      }
      newYValues[i][j] = accumulator;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      setYPosition(i, j, newYValues[i][j]);
    }
  }

  // Ensure the vertex normals reflect the new positions.
  updateNormals();
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
  GLuint kernelSize = kernel.size();
  glm::vec3 accumulator;
  std::vector<std::vector<glm::vec3>> newColours(size,
                          std::vector<glm::vec3>(size));

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      accumulator = glm::vec3(0.0f);

      for (GLuint k = 0; k < kernelSize; k++) {
        for (GLuint l = 0; l < kernelSize; l++) {
          accumulator += colours[(i + (k - (kernelSize / 2))) % size]
                                [(j + (l - (kernelSize / 2))) % size] *
                         kernel[k][l];
        }
      }
      newColours[i][j] = accumulator;
    }
  }

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      colours[i][j] = newColours[i][j];
    }
  }
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

/**
 * Add a specified amount of (greyscale) noise to vary the vertex colours.
 */
GLvoid Fractal::addColourNoise(GLfloat noiseLevel)
{
  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      GLfloat randomNoise = randomNumber(-noiseLevel, noiseLevel);

      colours[i][j].r += randomNoise;
      colours[i][j].g += randomNoise;
      colours[i][j].b += randomNoise;
    }
  }
}