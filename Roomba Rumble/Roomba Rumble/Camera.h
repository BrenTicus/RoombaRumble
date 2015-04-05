#pragma once

#include "GL/glew.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Camera{
private:
	glm::vec3 cameraPosition, cameraTarget, cameraUp;

public:
	Camera();
	~Camera();

	glm::vec3 getPosition();
	glm::vec3 getTarget();
	glm::vec3 getUp();

	void setup(glm::quat rotation, glm::vec3 target);
	void update(float timestep, glm::quat rotation, glm::vec3 target);
};