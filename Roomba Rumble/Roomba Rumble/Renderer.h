#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"

class Renderer
{
private:
	GLFWwindow* window;
public:
	Renderer();
	~Renderer();

	int Update();
};

