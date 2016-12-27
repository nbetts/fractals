/**
 * [Program description]
 */

#include "shader.hpp"
#include "helpers.hpp"

/**
 * Constructor to read and create the shader.
 */
Shader::Shader(const GLchar* vertexFile, const GLchar* fragmentFile,
               const GLchar* geometryFile)
{
  GLint compileStatus;
  GLchar compileLog[LOG_MSG_LENGTH];

  // 1. Retrieve the vertex/fragment source code from the given file paths.
  const GLchar* vertexShaderSource = readFile(vertexFile);
  const GLchar* fragmentShaderSource = readFile(fragmentFile);
  const GLchar* geometryShaderSource = readFile(geometryFile);

  // 2. Compile and compile the shaders.

  // Vertex shader
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShaderID);
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(vertexShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nVertex compilation error (ID: %d) in file: %s\n%s\n",
           vertexShaderID, vertexFile, compileLog);

    exit(EXIT_FAILURE);
  }

  // Fragment shader
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShaderID);
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(fragmentShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nFragment compilation error (ID: %d) in file: %s\n%s\n",
           fragmentShaderID, fragmentFile, compileLog);

    exit(EXIT_FAILURE);
  }

  // Geometry shader
  GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometryShaderID, 1, &geometryShaderSource, NULL);
  glCompileShader(geometryShaderID);
  glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(geometryShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nGeometry compilation error (ID: %d) in file: %s\n%s\n",
           geometryShaderID, geometryFile, compileLog);

    exit(EXIT_FAILURE);
  }

  // 3. Create and compile the shader program.
  programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  // glAttachShader(programID, geometryShaderID);
  glLinkProgram(programID);
  glGetProgramiv(programID, GL_LINK_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetProgramInfoLog(programID, LOG_MSG_LENGTH, NULL, compileLog);
    fprintf(stderr, "\nShader program compilation error (ID: %d):\n%s\n",
           programID, compileLog);

    exit(EXIT_FAILURE);
  }

  // Delete the shaders as they are now linked to the program.
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDetachShader(programID, geometryShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);
  glDeleteShader(geometryShaderID);
}

/**
 * Set attributes on the shader program by specifying the layout of vertex
 * data to use.
 */
GLvoid Shader::setAttributes(GLint attributeCount,
                             const GLchar** attributeNames,
                             GLint* attributeSizes)
{
  GLint i, stride = 0, offset = 0;

  for (i = 0; i < attributeCount; i++) {
    stride += attributeSizes[i];
  }

  for (i = 0; i < attributeCount; i++) {
    GLint attribute = glGetAttribLocation(programID,
                                          attributeNames[i]);
    glVertexAttribPointer(attribute, attributeSizes[i], GL_FLOAT,
                          GL_FALSE, stride * sizeof(GLfloat),
                          (GLvoid*)(offset * sizeof(GLfloat)));
    glEnableVertexAttribArray(attribute);
    offset += attributeSizes[i];
  }
}