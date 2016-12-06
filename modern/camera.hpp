#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"

class Camera
{
  public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    GLfloat yaw;
    GLfloat pitch;
    GLfloat movementSpeed;
    GLfloat turnSensitivity;
    GLfloat fov;

    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLfloat yaw,
           GLfloat pitch, GLfloat movementSpeed, GLfloat turnSensitivity,
           GLfloat fov);
    glm::mat4 getLookAtMatrix();
    GLvoid setYaw(GLfloat yaw);
    GLvoid setPitch(GLfloat pitch);
    GLvoid setMovementSpeed(GLfloat movementSpeed);
    GLvoid setTurnSensitivity(GLfloat turnSensitivity);
    GLvoid setFov(GLfloat fov);
    GLfloat getYaw();
    GLfloat getPitch();
    GLfloat getMovementSpeed();
    GLfloat getTurnSensitivity();
    GLfloat getFov();
};

#endif