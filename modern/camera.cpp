#include "camera.hpp"
#include <string>

/**
 * Constructor to create and set the attributes on the camera.
 */
Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLfloat yaw,
               GLfloat pitch, GLfloat movementSpeed, GLfloat turnSensitivity,
               GLfloat fov)
{
  position = position;
  front = front;
  up = up;

  setYaw(yaw);
  setPitch(pitch);
  setMovementSpeed(movementSpeed);
  setTurnSensitivity(turnSensitivity);
  setFov(fov);
}

glm::mat4 Camera::getLookAtMatrix()
{
  return glm::lookAt(position, position + front, up);
}

GLvoid Camera::setYaw(GLfloat desiredYaw)
{
  yaw = desiredYaw;
}

GLvoid Camera::setPitch(GLfloat desiredPitch)
{
  pitch = desiredPitch;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  } else if (pitch < -89.0f) {
    pitch = -89.0f;
  }
}

GLvoid Camera::setMovementSpeed(GLfloat desiredMovementSpeed)
{
  movementSpeed = desiredMovementSpeed;
}

GLvoid Camera::setTurnSensitivity(GLfloat desiredTurnSensitivity)
{
  turnSensitivity = desiredTurnSensitivity;
}

GLvoid Camera::setFov(GLfloat desiredFov)
{
  fov = desiredFov;

  if (fov <= 1.0f) {
    fov = 1.0f;
  } else if (fov >= 180.0f) {
    fov = 180.0f;
  }
}

GLfloat Camera::getYaw()
{
  return yaw;
}

GLfloat Camera::getPitch()
{
  return pitch;
}

GLfloat Camera::getMovementSpeed()
{
  return movementSpeed;
}

GLfloat Camera::getTurnSensitivity()
{
  return turnSensitivity;
}

GLfloat Camera::getFov()
{
  return fov;
}