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
	
	vector<GraphicsObject*> gObjList, staticList, powerupList, attachments, aiRoombas;
	GraphicsObject *ball, *shuriken, *airplane;
	GraphicsObject *roomba;
	glm::vec3 roombaPosition;
	GLfloat health, timeBuffer;
	GLint damage, kills, gameTime;
	glm::mat4 modelView, projection;
	float timestep;

	Camera* mainCamera;

public:
	Renderer(int gameTime);
	~Renderer();

	GLint getGameTime(){ return gameTime; }
	GLint getKillCount(){ return kills; }

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

