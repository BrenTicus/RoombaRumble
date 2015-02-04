#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include "stdio.h"
#include <iostream>

class Renderer
{
private:
	GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr;
	GLuint fragShaderPtr;
public:
	Renderer();
	~Renderer();

	bool readShader(const char* filename, int shaderType);
	int setupShaders();
	void Update();
};

