#include "shader.hpp"

/**
 * Constructor to read and create the shader.
 */
Shader::Shader(const GLchar* vertexFile, const GLchar* fragmentFile)
{
  GLint compileStatus;
  GLchar compileLog[LOG_MSG_LENGTH];

  // 1. Retrieve the vertex/fragment source code from the given file paths.
  const GLchar* vertexShaderSource = readFile(vertexFile);
  const GLchar* fragmentShaderSource = readFile(fragmentFile);

  // 2. Compile and compile the shaders.
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShaderID);
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(vertexShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    printf("Vertex compilation error (ID: %d) in file: %s\n%s\n",
           vertexShaderID, vertexFile, compileLog);

    exit(EXIT_FAILURE);
  }

  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShaderID);
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetShaderInfoLog(fragmentShaderID, LOG_MSG_LENGTH, NULL, compileLog);
    printf("Fragment compilation error (ID: %d) in file: %s\n%s\n",
           fragmentShaderID, fragmentFile, compileLog);

    exit(EXIT_FAILURE);
  }

  // 3. Create and compile the shader program.
  programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  // glBindFragDataLocation(programID, 0, "color");
  glLinkProgram(programID);
  glGetProgramiv(programID, GL_LINK_STATUS, &compileStatus);

  if (compileStatus != GL_TRUE) {
    glGetProgramInfoLog(programID, LOG_MSG_LENGTH, NULL, compileLog);
    printf("Shader program compilation error (ID: %d):\n%s\n",
           programID, compileLog);

    exit(EXIT_FAILURE);
  }

  // Delete the shaders as they are now linked to the program.
  glDetachShader(programID, vertexShaderID);
  glDetachShader(programID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);
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

/**
 * Use the shader program.
 */
GLvoid Shader::use()
{
  glUseProgram(programID);
}

/**
 * Get the shader program ID.
 */
GLuint Shader::getProgramID()
{
  return programID;
}

/**
 * Read the whole content of a given file into a char array.
 */
const GLchar* Shader::readFile(const GLchar* filename)
{
  using namespace std;

  ifstream file(filename);
  stringstream buffer;
  string* contents;

  buffer << file.rdbuf();
  file.close();
  contents = new string(buffer.str());

  return contents->c_str();
}
