#pragma once
#include "PhysicsManager.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace glm;
using namespace std;

struct obj {
	vector<GLfloat>* vertices;
	vector<GLfloat>* normals;
	vector<GLfloat>* texVertices;
	vector<GLuint>* faceIndices;
	vector<GLuint>*	normIndices;
	vector<GLuint>* texIndices;
};

const float BOTTOM_KILL_THRESHOLD = -10.0f;

class Entity
{
private:
	const char* tag;
	string powerupID;

protected:
	obj* model;
	PhysicsManager* physicsManager;
	PxMaterial* material;
	PxRigidDynamic* hitbox;
	vec3 position;
	quat rotation;
	bool destroy;
public:
	bool justAdded;
	int powerupType;

	Entity();
	Entity(PhysicsManager* physicsManager, vec3 position);
	
	virtual void Destroy();
	virtual int Update();

	bool isDestroyed() { return destroy; }
	void destroyFlag() { destroy = true; }
	quat getRotation();
	vec3 getPosition();
	vec3 getSpeed();
	obj* getModel();
	int readObj(obj* target, string filename);
	vector<PxVec3> objToVectors(obj* model);
	vector<PxVec3> objToVectors(obj* model, vector<PxU32>* faceOut);

	void setTag(const char* newTag){ tag = newTag;}
	void setPowerupID(string newID){ powerupID = newID; }
	const char* getTag(){ return tag;}
	string getPowerupID(){ return powerupID; }

};

