#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "ResourceManager.h"
#include "GraphicsObject.h"
#include "stdio.h"
#include <iostream>

class GUI{
private:
	glm::mat4 projection, modelView;
	GLfloat wWidth, wHeight;
	GLuint* shaderIDs;
	GLfloat maxHP;
	ResourceManager* rManager;
	GraphicsObject* wordKills;

public:
	GUI(GLuint width, GLuint height, GLuint *shaders);
	~GUI();

	glm::mat4 getProj(){ return projection; }
	glm::mat4 getMV(){ return modelView; }

	vector<GLfloat> myTranslate(vector<GLfloat> verts, GLfloat deltaX, GLfloat deltaY);
	vector<GLfloat> refactorNormals(vector<GLfloat> normals);
	void bindBuffer(GLuint &VAO, GLuint &VBO);
	void drawWord(const char* key);
	GLboolean drawHealth(GLfloat health);
};