#pragma once
#include "PhysicsManager.h"
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "ResourceManager.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace glm;
using namespace std;


const float BOTTOM_KILL_THRESHOLD = -10.0f;

class Entity
{
private:
	const char* tag;

protected:
	obj* model;
	PhysicsManager* physicsManager;
	ResourceManager* resourceManager;
	PxMaterial* material;
	PxRigidDynamic* hitbox;
	vec3 position;
	quat rotation;
	bool destroy;
	string powerupID;
public:
	bool justAdded;
	int powerupType, pIndex;

	Entity();
	Entity(vec3 position);
	
	virtual void Destroy();
	virtual int Update();

	bool isDestroyed() { return destroy; }
	void destroyFlag() { destroy = true; }
	void aliveFlag() { destroy = false; }
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

