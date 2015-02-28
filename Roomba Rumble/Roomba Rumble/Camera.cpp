#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::vec3 Camera::getPosition()
{
	return cameraPosition;
}
glm::vec3 Camera::getTarget()
{
	return cameraTarget;
}
glm::vec3 Camera::getUp()
{
	return cameraUp;
}

void Camera::setup(glm::quat rotation, glm::vec3 target)
{
	glm::vec4 translate(0.0f, 2.5f, -7.5f, 0.0f);
	glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

	translate = rotation * translate;
	up = rotation * up;

	cameraPosition = target + glm::vec3(translate);
	cameraTarget = target;
	cameraUp = glm::vec3(up);
}