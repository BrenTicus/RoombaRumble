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
#include "game.h"

const string playerNames [NUM_ROOMBAS] = {
	string("Player One"),
	string("Taco"),
	string("Frank"),
	string("Jessica"),
	string("George"),
	string("Cindy"),
	string("Keith"),
	string("Angela")
};

class Renderer
{
private:
	EntityManager* eManager;
	ResourceManager* rManager;
	RendererInfoFetcher rif;
	Skybox* skybox;
	GUI* gui;

	static GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;
	
	vector<GraphicsObject*> gObjList, staticList, powerupList, attachments;
	GraphicsObject *ball, *shuriken, *airplane;
	glm::vec3 roombaPosition;
	GLfloat health, timeBuffer;
	GLint kills, gameTime, pType, pLevel;
	glm::mat4 modelView, projection;
	float timestep;
	GLint gameOver;
	GLboolean justEnded;
	GLfloat wWidth, wHeight;
	vector<scoreID> scoreBoard;

	Camera* mainCamera;

public:

	Renderer(int gameTime, GLuint width, GLuint height);
	~Renderer();

	GLint getGameOver(){ return gameOver; }
	GLint getGameTime(){ return gameTime; }
	GLint getKillCount(){ return kills; }

	GLboolean readShader(const char* filename, int shaderType);
	GLint setupShaders();
	void setupObjectsInScene();

	void gameOverState();
	void updateScene();
	void drawScene(int width, int height);
	void menu(int _menu, int pos);
	void Update();
	void destroyObjects();

	void clearObjData();
	GLFWwindow* getWindow();

	GUI* getGUI(){ return gui;}

	Camera* getCamera(){ return mainCamera;}
};

