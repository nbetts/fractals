/**
 * [Program description]
 */

#ifndef SHADER_HEADER
#define SHADER_HEADER

#define LOG_MSG_LENGTH 256

class Shader
{
  public:
    GLuint programID;

    typedef enum {
      FRACTAL,
      NONE // only used for enum iteration
    } ShaderType;
    
    Shader(std::string vertexFile, std::string fragmentFile,
           std::string geometryFile);
    GLvoid setAttributes(GLint attributeCount, const GLchar** attributeNames,
                         GLint* attributeSizes);
};
  
#endif
