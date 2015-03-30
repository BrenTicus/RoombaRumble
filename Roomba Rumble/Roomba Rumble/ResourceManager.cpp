#include "ResourceManager.h"

ResourceManager* ResourceManager::mainResourceManager = NULL;

int ResourceManager::readObj(obj* target, string filename) {
	char ch;
	string str;
	GLfloat f;
	GLuint i;
	ifstream reader(filename);

	// Reset everything in the target.
	target->faceIndices = new vector<GLuint>(0);
	target->normIndices = new vector<GLuint>(0);
	target->texIndices = new vector<GLuint>(0);
	target->normals = new vector<GLfloat>(0);
	target->texVertices = new vector<GLfloat>(0);
	target->vertices = new vector<GLfloat>(0);

	if (reader.is_open()) {
		while (!reader.eof()) {
			reader >> str;

			if (str == "v") {
				reader >> f;
				target->vertices->push_back(f);

				reader >> f;
				target->vertices->push_back(f);

				reader >> f;
				target->vertices->push_back(f);

				target->vertices->push_back(1.0f);
			}
			if (str == "vn"){
				reader >> f;
				target->normals->push_back(f);

				reader >> f;
				target->normals->push_back(f);

				reader >> f;
				target->normals->push_back(f);
			}
			if (str == "vt"){
				reader >> f;
				target->texVertices->push_back(f);

				reader >> f;
				target->texVertices->push_back(f);
			}
			if (str == "f") {
				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);

				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);

				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);
			}
			str = "";
		}
		reader.close();
	}
	else{
		cout << "Couldn't open file \"" << filename << "\"" << endl;
		return 1;
	}

	return 0;
}


//Read in all the models
void ResourceManager::initialize(){
	roomba = new obj();
	level = new obj();
	powerupShield = new obj();
	powerupShieldLvl2 = new obj();
	powerupShieldLvl3 = new obj();
	powerupRange = new obj();
	powerupRangeLvl2 = new obj();
	powerupRangeLvl3 = new obj();
	powerupMelee = new obj();
	powerupMeleeLvl2 = new obj();
	powerupMeleeLvl3 = new obj();
	powerupHealth = new obj();
	projectile = new obj();
	wheel = new obj();

	readObj(roomba, "Assets/roomba.obj");
	readObj(level, "Assets/level0.obj");
	readObj(powerupShield, "Assets/shield_1.obj");
	readObj(powerupShieldLvl2, "Assets/shield_2.obj");
	readObj(powerupShieldLvl3, "Assets/shield_3.obj");
	readObj(powerupRange, "Assets/ranged_1.obj");
	readObj(powerupRangeLvl2, "Assets/ranged_2.obj");
	readObj(powerupRangeLvl3, "Assets/ranged_3.obj");
	readObj(powerupMelee, "Assets/melee_1.obj");
	readObj(powerupMeleeLvl2, "Assets/melee_2.obj");
	readObj(powerupMeleeLvl3, "Assets/melee_3.obj");
	readObj(powerupHealth, "Assets/Health.obj");
	readObj(projectile, "Assets/projectile_1.obj");
	readObj(wheel, "Assets/wheel.obj");
}

ResourceManager::ResourceManager(){
	if (mainResourceManager == NULL) mainResourceManager = this;
	initialize();
}

ResourceManager::~ResourceManager()
{
	if (mainResourceManager == this) mainResourceManager = NULL;
}
