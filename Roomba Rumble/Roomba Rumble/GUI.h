#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "ResourceManager.h"
#include "GraphicsObject.h"
#include "stdio.h"
#include <algorithm>
#include <iostream>

const int KILLS_TO_WIN = 500000;
const int NUM_MENUS = 3;

const GLfloat spaceWidth = 0.3f;
const GLfloat wordHeight = 20.0f;

const vec3 RED = vec3(1.0f, 0.0f, 0.0f);
const vec3 GREEN = vec3(0.0f, 1.0f, 0.0f);
const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);
const vec3 WHITE = vec3(1.0f, 1.0f, 1.0f);

#define MAIN_MENU 0
#define PAUSE_MENU 1
#define GAME_OVER_MENU 2

struct myTime{
	GLuint minuteLeft, minuteRight;
	GLuint secondLeft, secondRight;
};

struct scoreID{
	GLuint index;
	GLint score;
};

class GUI{
private:
	glm::mat4 projection, modelView;
	GLfloat wWidth, wHeight;
	GLuint* shaderIDs;
	GLfloat maxHP;
	vec3 damWordTrans, killWordTrans, timeTrans, killsTrans, damageTrans;
	vec3 respawnMessageTrans, winMessageTrans, loseMessageTrans;
	ResourceManager* rManager;
	GraphicsObject* numbers[10];
	map<char, GraphicsObject*> letters;
	vector<GLfloat> menuBacking;
	obj* backing;
	GraphicsObject* menu[NUM_MENUS];

	bool respawning;
public:
	GUI(GLuint width, GLuint height, GLuint *shaders);
	~GUI();

	glm::mat4 getProj(){ return projection; }
	glm::mat4 getMV(){ return modelView; }

	vector<GLfloat> refactorNormals(vector<GLfloat> normals);
	vector<GraphicsObject*> fetchWord(string word);
	myTime getTime(GLuint bulk);
	GLfloat getWordWidth(string word, GLfloat scale);
	vector<GLfloat> getMenuBacking(GLfloat width, GLfloat height);

	void loadObjects();
	void bindBuffer(GLuint &VAO, GLuint &VBO);
	void drawWord(string key, vec3 ambient, vec3 translate, GLfloat scalarX, GLfloat scalarY);
	void drawTime(GLint gameTime, vec3 ambient, vec3 scaleVec);
	void drawMenu(GLuint menuIndex);
	vector<scoreID> sortScores(vector<scoreID> scoreBoard);
	void drawStaticElements(GLint gameOver);
	void drawDynamicElements(GLint gameTime, GLint damage, GLint kills, GLfloat health, vector<scoreID> scoreBoard);
	GLboolean drawHealth(GLfloat health);

	void showRespawning();
	void hideRespawning();
};