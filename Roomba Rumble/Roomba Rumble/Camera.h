#pragma once

#include "GL/glew.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Camera{
private:
	glm::vec3 cameraPosition, cameraTarget, cameraUp, cameraForward, cameraVelocity;
	glm::vec3 lastPosition;
	float lastTimestep;

public:
	Camera();
	~Camera();

	glm::vec3 getPosition();
	glm::vec3 getTarget();
	glm::vec3 getUp();
	glm::vec3 getForward();
	glm::vec3 getVelocity();

	void setup(glm::quat rotation, glm::vec3 target);
	void update(float timestep, glm::quat rotation, glm::vec3 target);

	glm::vec3 getForwardVector(glm::quat q);
};