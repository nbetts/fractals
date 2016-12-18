/**
 * [Program description]
 */

#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include "glm/glm/glm.hpp"

class Camera
{
  public:
    typedef enum {
      FORWARD,
      BACKWARD,
      LEFT,
      RIGHT
    } Direction;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    glm::mat4 view;
    glm::mat4 projection;

    GLfloat yaw;
    GLfloat pitch;
    GLfloat movementSpeed;
    GLfloat turnSensitivity;
    GLfloat fov;

    Camera(glm::vec3 desiredPosition, glm::vec3 desiredFront,
           glm::vec3 desiredUp, GLfloat yaw, GLfloat pitch,
           GLfloat movementSpeed, GLfloat turnSensitivity, GLfloat fov);
    glm::mat4 getLookAtMatrix();
    GLvoid updatePosition(Direction direction, GLfloat deltaTime);
    GLvoid updateOrientation(GLfloat newYaw, GLfloat newPitch);
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
    GLvoid printPosition();
};

#endif
