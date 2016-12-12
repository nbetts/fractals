/**
 * [Program description]
 */

#include "fractal.hpp"
#include "helpers.hpp"

/**
 * Constructor to initialise the fractal with the given properties.
 */
Fractal::Fractal(GLuint desiredDepth, GLfloat desiredYRange,
                 GLfloat desiredYDeviance, glm::vec3 dColour)
{
  depth = desiredDepth;
  size = 2 << (depth - 1);
  yRange = desiredYRange;
  yRangeIncrement = 0.02f;
  yDeviance = desiredYDeviance;
  yDevianceIncrement = 0.02f;
  colour = dColour;

  vertexCount = size * size;
  vertexData = new GLfloat[vertexCount * 6];
  rawYValues = new GLfloat[vertexCount];

  indexCount = (size - 1) * (size - 1) * 6;
  indexData = new GLuint[indexCount];
}

/**
 * Set the Y value at a given vertex in the 
 */
GLvoid Fractal::setY(GLuint x, GLuint z, GLfloat value)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) *
               size;

  rawYValues[position] = value;
  vertexData[(position * 6) + 1] = value;
}

/**
 * Get the Y value at a given vertex in the 
 */
GLfloat Fractal::getY(GLuint x, GLuint z)
{
  GLuint position = (x & (size - 1)) + (z & (size - 1)) *
               size;

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

  // Generate the X, Z and color values of each vertex.
  GLuint i, j, counter = 0;
  GLfloat yDifference = yRange * 2.0f;

  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      GLfloat yValue = getY(i, j);
      vertexData[counter++] = (GLfloat)(i % (GLuint)size) /
                                      (GLfloat)size;
      vertexData[counter++] = yValue;
      vertexData[counter++] = (GLfloat)j / (GLfloat)size;
      vertexData[counter++] = colour.x;
      vertexData[counter++] = colour.y;
      vertexData[counter++] = (yValue / yDifference) + colour.z;
    }
  }

  // Generate the indices.
  counter = 0;
  GLuint increment = 0;

  for (i = 0; i <= indexCount + 6; i += 6) {
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
}

/**
 * Perform a convolution with a given kernel to smooth the fractal.
 */
GLvoid Fractal::convolve(GLuint kernelSize, GLfloat** kernel)
{
  GLfloat newYValues[size][size];
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