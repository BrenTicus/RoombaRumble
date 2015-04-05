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

const int KILLS_TO_WIN = 5;

const vec3 killWordTrans = vec3(40.0f, 0.0f, 0.0f);
const vec3 damWordTrans = vec3(1.0f, 35.0f, 0.0f);
const vec3 killsTrans = vec3(46.0f, 0.0f, 0.0f);
const vec3 damageTrans = vec3(10.0f, 35.0f, 0.0f);

const vec3 minLeftTrans = vec3(30.0f, 0.0f, 0.0f);
const vec3 minRightTrans = vec3(31.5f, 0.0f, 0.0f);
const vec3 colonTrans = vec3(33.0f, 0.0f, 0.0f);
const vec3 secLeftTrans = vec3(34.0f, 0.0f, 0.0f);
const vec3 secRightTrans = vec3(35.5f, 0.0f, 0.0f);

struct myTime{
	GLuint minuteLeft, minuteRight;
	GLuint secondLeft, secondRight;
};

class GUI{
private:
	glm::mat4 projection, modelView;
	GLfloat wWidth, wHeight;
	GLuint* shaderIDs;
	GLfloat maxHP;
	ResourceManager* rManager;
	GraphicsObject *wordKills, *wordDamage, *symbolColon;
	GraphicsObject* numbers[10];

public:
	GUI(GLuint width, GLuint height, GLuint *shaders);
	~GUI();

	glm::mat4 getProj(){ return projection; }
	glm::mat4 getMV(){ return modelView; }

	vector<GLfloat> refactorNormals(vector<GLfloat> normals);
	myTime getTime(GLuint bulk);
	void loadObjects();
	void bindBuffer(GLuint &VAO, GLuint &VBO);
	void drawWord(const char* key);
	void drawTime(GLint gameTime, vec3 ambient, vec3 scaleVec);
	void drawStaticElements();
	void drawDynamicElements(GLint gameTime, GLint damage, GLint kills);
	GLboolean drawHealth(GLfloat health);
};