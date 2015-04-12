#include "Camera.h"
#include "PhysicsManager.h"
#include <iostream>
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
glm::vec3 Camera::getForward(){
	return cameraForward;
}

//Returns velocity in meters per second
glm::vec3 Camera::getVelocity(){
	return cameraVelocity;
}

//Taken from: http://nic-gamedev.blogspot.ca/2011/11/quaternion-math-getting-local-axis.html?m=1
glm::vec3 Camera::getForwardVector(glm::quat q) 
{
	return glm::vec3( 2 * (q.x * q.z + q.w * q.y), 
		2 * (q.y * q.x - q.w * q.x),
		1 - 2 * (q.x * q.x + q.y * q.y));
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
	}

	cameraPosition = target + glm::vec3(translate);
	lastPosition = cameraPosition;
	lastTimestep = 0;

	cameraTarget = target;
	cameraUp = glm::vec3(up);
	cameraForward = glm::vec3(getForwardVector(rotation));
	cameraVelocity = glm::vec3(0,0,0);
}

void Camera::update(float timestep, glm::quat rotation, glm::vec3 target)
{
	glm::vec4 translate;
	glm::vec4 up(0.0f, 1.0f, 0.0f, 0.0f);

	if (target == glm::vec3(0.0f, -20.0f, 0.0f))
	{
		target == glm::vec3(0.0f);
		translate = glm::vec4(0.0f, 120.0f, 1.0f, 0.0f);
	}
	else
	{
		translate = glm::vec4(0.0f, 2.5f, -7.5f, 0.0f);
		translate = rotation * translate;
		//up = rotation * up;
	}

	//compute velocity
	lastPosition = cameraPosition;
	


	glm::vec3 newTarget = target + glm::vec3(translate);
	newTarget = (newTarget - cameraPosition) * 0.08f;
	if(glm::length(newTarget) > 0.1) cameraPosition += newTarget;
	
	float currentTime = (float) clock();
	cameraVelocity = (lastPosition - cameraPosition) / ((lastTimestep - currentTime) /1000.0f);			//meters per second required
	lastTimestep = currentTime;


	cameraTarget = target;
	cameraUp = glm::vec3(up);
	cameraForward = glm::vec3(getForwardVector(rotation));
	//printf("velo Z %f Y %f X %f\n", cameraVelocity.z, cameraVelocity.y, cameraVelocity.x);
	//printf("POS Z %f Y %f X %f\n", cameraPosition.z, cameraPosition.y, cameraPosition.x);
	//printf("FOR Z %f Y %f X %f\n", cameraForward.z, cameraForward.y, cameraForward.x);

	
}