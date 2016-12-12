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

  // rawYValues = new GLfloat[vertexCount];
  // positions = new GLfloat[vertexCount];
  // normals = new GLfloat[vertexCount];
  // colours = new GLfloat[vertexCount];
  // indexData = new GLuint[indexCount];
  // vertexData = new GLfloat[vertexCount * DIMENSIONS * attributeCount];

  vertexCount = size * size;
  vertexData = new GLfloat[vertexCount * 9];
  rawYValues = new GLfloat[vertexCount];

  indexCount = (size - 1) * (size - 1) * 6;
  indexData = new GLuint[indexCount];
}

/**
 * Set the Y value at a given vertex in the 
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) * size;

  rawYValues[position] = value;
  // positions[(position * DIMENSIONS) + 1] = value;
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

  generateVertexData();
  // updateColours();
  updateNormals();
  generateIndexData();

  // updatePositions();
  // updateNormals();
  // updateColours();
  // generateIndexData();
  // generateVertexData();
}

/**
 * Generate the index data.
 */
GLvoid Fractal::generateIndexData()
{
  GLuint i, counter = 0, increment = 0;

  for (i = 0; i <= indexCount + (2 * DIMENSIONS); i += (2 * DIMENSIONS)) {
    if (increment != 0 && (increment + 1) % size == 0) {
      increment++;
      continue;
    }

    indexData[counter++] = increment;
    indexData[counter++] = increment + 1;
    indexData[counter++] = increment + size;

    indexData[counter++] = increment + size;
    indexData[counter++] = increment + 1;
    indexData[counter++] = increment + 1 + size;

    increment++;
  }

  // GLuint offset = 0, increment = 0;

  // for (GLuint i = 0; i < size - 1; i++) {
  //   for (GLuint j = 0; j < size - 1; j++) {
  //     indexData[offset++] = increment;
  //     indexData[offset++] = increment + 1;
  //     indexData[offset++] = increment + size;

  //     indexData[offset++] = increment + size;
  //     indexData[offset++] = increment + 1;
  //     indexData[offset++] = increment + size + 1;

  //     increment++;
  //   }
  // }
}

/**
 * Generate the vertex data.
 */
GLvoid Fractal::generateVertexData()
{
  // Update the positions and generate the color values of each vertex.
  GLuint i, j, counter = 0;
  GLfloat yDifference = yRange * 2.0f;

  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      GLfloat yValue = getY(i, j);
      vertexData[counter++] = (GLfloat)(i % (GLuint)size) /
                                            (GLfloat)size;
      vertexData[counter++] = yValue;
      vertexData[counter++] = (GLfloat)j / (GLfloat)size;

      counter += DIMENSIONS;

      vertexData[counter++] = baseColour.x;
      vertexData[counter++] = (yValue / yDifference) + (baseColour.y / 2.0f);
      vertexData[counter++] = baseColour.z;
    }
  }


  // GLuint offset = 0;

  // for (GLuint i = 0; i < vertexCount; i++) {
  //   vertexData[offset++] = positions[i + 0];
  //   vertexData[offset++] = positions[i + 1];
  //   vertexData[offset++] = positions[i + 2];

  //   vertexData[offset++] = normals[i + 0];
  //   vertexData[offset++] = normals[i + 1];
  //   vertexData[offset++] = normals[i + 2];

  //   vertexData[offset++] = colours[i + 0];
  //   vertexData[offset++] = colours[i + 1];
  //   vertexData[offset++] = colours[i + 2];
  // }
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
    }
  }
}

/**
 * Update the normal of each vertex.
 */
GLvoid Fractal::updateNormals()
{
  GLuint counter = 0;
  glm::vec3 p1, p2, p3, v1, v2, normal;

  for (GLuint i = 0; i < size; i++) {
    for (GLuint j = 0; j < size; j++) {
      p1 = glm::vec3(i, getY(i, j), j);
      p2 = glm::vec3(i, getY(i, j + 1), j + 1);
      p3 = glm::vec3(i + 1, getY(i + 1, j), j);

      v1 = p2 - p1,
      v2 = p3 - p1;

      normal.x = (v1.y * v2.z) - (v1.z - v2.y);
      normal.y = -((v2.z * v1.x) - (v2.x * v1.z));
      // normal.y = (v2.z * v1.x) - (v2.x * v1.z);
      normal.z = (v1.x * v2.y) - (v1.y * v2.x);
      normal = glm::normalize(normal);

      counter += DIMENSIONS;

      // vertexData[counter++] = normal.x;
      // vertexData[counter++] = normal.y;
      // vertexData[counter++] = normal.z;
      vertexData[counter++] = 0.0f;
      vertexData[counter++] = 1.0f;
      vertexData[counter++] = 0.0f;

      counter += DIMENSIONS;
    }
  }

  // GLuint offset = 0;
  // glm::vec3 p1, p2, p3, v1, v2, normal;

  // for (GLuint i = 0; i < size; i++) {
  //   for (GLuint j = 0; j < size; j++) {
  //     p1 = glm::vec3(i, getY(i, j), j);
  //     p2 = glm::vec3(i, getY(i, j + 1), j + 1);
  //     p3 = glm::vec3(i + 1, getY(i + 1, j), j);

  //     v1 = p2 - p1,
  //     v2 = p3 - p1;

  //     normal.x = (v1.y * v2.z) - (v1.z - v2.y);
  //     normal.y = (v2.z * v1.x) - (v2.x * v1.z);
  //     normal.z = (v1.x * v2.y) - (v1.y * v2.x);
  //     normal = glm::normalize(normal);

  //     normals[offset++] = normal.x;
  //     normals[offset++] = normal.y;
  //     normals[offset++] = normal.z;
  //   }
  // }
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
      colours[offset++] = (yValue / yRange) + (baseColour.y / 2.0f);
      colours[offset++] = baseColour.z;
    }
  }
}

/**
 * Perform a convolution with a given kernel to smooth the fractal.
 */
GLvoid Fractal::convolve(GLuint kernelSize, GLfloat** kernel)
{
  // GLfloat newYValues[size][size];
  // GLuint centerX = size / 2;
  // GLuint centerY = size / 2;

  // GLuint ii, jj, mm, nn;

  // for (GLuint i = 0; i < size; i++) {
  //   for (GLuint j = 0; j < size; j++) {
  //     for (GLuint k = 0; k < kernelSize; k++) {
  //       for (GLuint l = 0; l < kernelSize; l++) {
  //         newYValues[i][j] = 0.0f;
  //       }
  //     }
  //   }
  // }

  // for (GLuint i = 0; i < size; i++) {
  //   for (GLuint j = 0; j < size; j++) {
  //     setY(i, j, newYValues[i][j]);
  //   }
  // }
}
