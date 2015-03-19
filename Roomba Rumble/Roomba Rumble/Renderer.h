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
#include <iostream>

class Renderer
{
private:
	EntityManager* eManager;
	RendererInfoFetcher rif;
	Skybox* skybox;

	GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;
	GLint ambientID, diffuseID, specAlbID, specPowID;
	GLint texObjID, mvMatID, projMatID, lightPosID;

	GLuint numStatObjs;
	
	vector<GraphicsObject> gObjList, staticList;
	GraphicsObject projectile;
	glm::vec3 roombaPosition;
	glm::mat4 modelView, projection;

public:
	Renderer(EntityManager* eManager);
	~Renderer();

	bool readShader(const char* filename, int shaderType);
	int setupShaders();
	void setupObjectsInScene();

	void addProjToScene();
	void updatePositions();
	void drawScene(int width, int height);
	void drawObject(GraphicsObject * gObj, vec3 scale, GLsizei count);
	void Update(EntityManager* eManager);
	void destroyObjects();

	void clearObjData();
	GLFWwindow* getWindow();
};

