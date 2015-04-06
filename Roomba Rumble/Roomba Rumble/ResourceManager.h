#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

const int NUM_LETTERS = 65;
const int ALPHABET_OFFSET = 58;

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
	void loadNumberObjs(string font);
	void loadLetterObjs(string font);
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
	obj* numbers[10];
	map<char, obj*> letters;

	ResourceManager();
	~ResourceManager();
};
