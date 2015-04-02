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
	vec3 killWordTrans, damWordTrans, killsTrans, damageTrans;
	ResourceManager* rManager;
	GraphicsObject *wordKills, *wordDamage;
	GraphicsObject* numbers[10];

public:
	GUI(GLuint width, GLuint height, GLuint *shaders);
	~GUI();

	glm::mat4 getProj(){ return projection; }
	glm::mat4 getMV(){ return modelView; }

	vector<GLfloat> refactorNormals(vector<GLfloat> normals);
	void loadObjects();
	void bindBuffer(GLuint &VAO, GLuint &VBO);
	void drawWord(const char* key);
	void drawStaticElements();
	void drawDynamicElements(GLint damage, GLint kills);
	GLboolean drawHealth(GLfloat health);
};