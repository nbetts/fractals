/**
 * [Program description]
 */

#ifndef SHADER_HEADER
#define SHADER_HEADER

#define LOG_MSG_LENGTH 256

class Shader
{
  // Shader program ID
  GLuint programID;

  public:
    typedef enum {
      FRACTAL,
      NONE // only used for enum iteration
    } ShaderType;
    
    Shader(const GLchar* vertexFile, const GLchar* fragmentFile);
    GLvoid setAttributes(GLint attributeCount, const GLchar** attributeNames,
                         GLint* attributeSizes);
    GLvoid use();
    GLuint getProgramID();
};
  
#endif
