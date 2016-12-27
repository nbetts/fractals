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
    
    Shader(const GLchar* vertexFile, const GLchar* fragmentFile,
           const GLchar* geometryFile);
    GLvoid setAttributes(GLint attributeCount, const GLchar** attributeNames,
                         GLint* attributeSizes);
};
  
#endif
