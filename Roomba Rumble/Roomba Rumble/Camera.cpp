#include "Camera.h"

const float MIN_HEIGHT = 1.0f;

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
	glm::vec4 translate;
	glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

	if(target == glm::vec3(0.0f, -20.0f, 0.0f))
	{
		target == glm::vec3(0.0f);
		translate = glm::vec4(0.0f, 120.0f, 1.0f, 0.0f);
	}
	else
	{
		translate = glm::vec4(0.0f, 2.5f, -7.5f, 0.0f);
		translate = rotation * translate;
		up = rotation * up;
	}

	cameraPosition = target + glm::vec3(translate);

	//if(cameraPosition.y < MIN_HEIGHT)
		//cameraPosition.y = MIN_HEIGHT;

	cameraTarget = target;
	cameraUp = glm::vec3(up);
}