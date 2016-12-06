#ifndef SHADER_HEADER
#define SHADER_HEADER

#include <string>
#include <fstream>
#include <sstream>

#define LOG_MSG_LENGTH 256

class Shader
{
  // Shader program ID
  GLuint programID;

  public:
    Shader(const GLchar* vertexFile, const GLchar* fragmentFile);
    GLvoid setAttributes(GLint attributeCount, const GLchar** attributeNames,
                         GLint* attributeSizes);
    GLvoid use();
    GLuint getProgramID();
    const GLchar* readFile(const GLchar* filename);
};
  
#endif
