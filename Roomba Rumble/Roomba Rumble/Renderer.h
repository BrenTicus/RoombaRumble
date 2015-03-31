#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Entity.h"
#include "EntityManager.h"
#include "Material.h"
#include "GraphicsObject.h"
#include "Camera.h"
#include "stdio.h"
#include "Skybox.h"
#include "GUI.h"
#include "ResourceManager.h"
#include <iostream>

class Renderer
{
private:
	EntityManager* eManager;
	ResourceManager* rManager;
	RendererInfoFetcher rif;
	Skybox* skybox;
	GUI *gui;

	GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;
	
	vector<GraphicsObject*> gObjList, staticList, powerupList, attachments;
	GraphicsObject *ball, *shuriken, *airplane;
	GraphicsObject *roomba, *airoomba;
	glm::vec3 roombaPosition;
	GLfloat health;
	glm::mat4 modelView, projection;

public:
	Renderer();
	~Renderer();

	GLboolean readShader(const char* filename, int shaderType);
	GLint setupShaders();
	void setupObjectsInScene();

	void updateScene();
	void drawScene(int width, int height);
	void Update();
	void destroyObjects();

	void clearObjData();
	GLFWwindow* getWindow();
};

