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

class Entity
{
protected:
	obj* model;
	PhysicsManager* physicsManager;
	PxMaterial* material;
	PxRigidDynamic* hitbox;
	vec3 position;
	quat rotation;
	bool destroy;
public:
	Entity();
	Entity(PhysicsManager* physicsManager, vec3 position);
	
	virtual void Destroy();
	virtual int Update();

	void destroyFlag() { destroy = true; }
	quat getRotation();
	vec3 getPosition();
	obj* getModel();
	int readObj(obj* target, string filename);
	vector<PxVec3> objToVectors(obj* model);
	vector<PxVec3> objToVectors(obj* model, vector<PxU32>* faceOut);
};

