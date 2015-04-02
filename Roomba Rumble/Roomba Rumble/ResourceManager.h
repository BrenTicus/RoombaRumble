#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct obj {
	vector<GLfloat>* vertices;
	vector<GLfloat>* normals;
	vector<GLfloat>* texVertices;
	vector<GLuint>* faceIndices;
	vector<GLuint>*	normIndices;
	vector<GLuint>* texIndices;
};

class ResourceManager{
private:

	void initialize();
	void loadNumberObjs();
	int readObj(obj* target, string filename);
public:
	static ResourceManager* mainResourceManager;

	obj* roomba;
	obj* level;
	obj* powerupShield;
	obj* powerupShieldLvl2;
	obj* powerupShieldLvl3;
	obj* powerupMelee;
	obj* powerupMeleeLvl2;
	obj* powerupMeleeLvl3;
	obj* powerupRange;
	obj* powerupRangeLvl2;
	obj* powerupRangeLvl3;
	obj* powerupHealth;
	obj* projectile;
	obj* projectileLvl2;
	obj* projectileLvl3;
	obj* wheel;
	obj* range;
	obj* wordKills;
	obj* wordDamage;
	obj* numbers[10];
	ResourceManager();
	~ResourceManager();
};
