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

const vec3 killWordTrans = vec3(850.0f, 55.0f, 0.0f);
const vec3 damWordTrans = vec3(10.0f, 740.0f, 0.0f);
const vec3 killsTrans = vec3(970.0f, 55.0f, 0.0f);
const vec3 damageTrans = vec3(220.0f, 740.0f, 0.0f);

const vec3 timeTrans = vec3(450.0f, 50.0f, 0.0f);

const vec3 winMessageTrans = vec3(220.0f, 450.0f, 0.0f);
const vec3 loseMessageTrans = vec3(200.0f, 450.0f, 0.0f);

const vec3 RED = vec3(1.0f, 0.0f, 0.0f);
const vec3 GREEN = vec3(0.0f, 1.0f, 0.0f);
const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);
const vec3 WHITE = vec3(1.0f, 1.0f, 1.0f);

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
	GraphicsObject* numbers[10];
	map<char, GraphicsObject*> letters;

public:
	GUI(GLuint width, GLuint height, GLuint *shaders);
	~GUI();

	glm::mat4 getProj(){ return projection; }
	glm::mat4 getMV(){ return modelView; }

	vector<GLfloat> refactorNormals(vector<GLfloat> normals);
	vector<GraphicsObject*> fetchWord(string word);
	myTime getTime(GLuint bulk);
	void loadObjects();
	void bindBuffer(GLuint &VAO, GLuint &VBO);
	void drawWord(string key, vec3 ambient, vec3 translate, GLfloat scalarX, GLfloat scalarY);
	void drawTime(GLint gameTime, vec3 ambient, vec3 scaleVec);
	void drawStaticElements(GLint gameOver);
	void drawDynamicElements(GLint gameTime, GLint damage, GLint kills, GLfloat health);
	GLboolean drawHealth(GLfloat health);
};