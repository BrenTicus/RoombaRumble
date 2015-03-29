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
	obj* roomba;
	obj* level;
	obj* powerupShield;
	obj* powerupMelee;
	obj* powerupRange;
	obj* powerupHealth;
	obj* projectile;
	obj* wheel;
	obj* range;
	ResourceManager();
};