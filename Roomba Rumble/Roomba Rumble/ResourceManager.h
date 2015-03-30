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
	int readObj(obj* target, string filename);
public:
	static ResourceManager* mainResourceManager;

	obj* roomba;
	obj* level;
	obj* powerupShield;
	obj* powerupMelee;
	obj* powerupMeleeLvl2;
	obj* powerupMeleeLvl3;
	obj* powerupRange;
	obj* powerupRangeLvl2;
	obj* powerupRangeLvl3;
	obj* powerupHealth;
	obj* projectile;
	obj* wheel;
	obj* range;
	ResourceManager();
	~ResourceManager();
};
