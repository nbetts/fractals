/**
 * [Program description]
 */

#include "camera.hpp"

/**
 * Constructor to create and set the attributes of the camera.
 */
Camera::Camera(glm::vec3 desiredPosition, glm::vec3 desiredFront,
               glm::vec3 desiredUp, GLfloat yaw, GLfloat pitch,
               GLfloat movementSpeed, GLfloat turnSensitivity, GLfloat fov)
{
  position = desiredPosition;
  front = desiredFront;
  up = desiredUp;

  view = glm::mat4();
  projection = glm::mat4();

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

GLvoid Camera::updatePosition(Direction direction, GLfloat distance)
{
  switch(direction) {
    case Camera::FORWARD:
      position += front * distance;
      break;
    case Camera::BACKWARD:
      position -= front * distance;
      break;
    case Camera::LEFT:
      position -= glm::normalize(glm::cross(front, up)) * distance;
      break;
    case Camera::RIGHT:
      position += glm::normalize(glm::cross(front, up)) * distance;
      break;
  }
}

GLvoid Camera::updateOrientation(GLfloat newYaw, GLfloat newPitch)
{
  setYaw(newYaw);
  setPitch(newPitch);

  front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  front.y = sin(glm::radians(pitch));
  front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
  front = glm::normalize(front);
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

GLvoid Camera::printPosition()
{
  printf("camera p: %.3f %.3f %.3f\n", position.x, position.y, position.z);
}