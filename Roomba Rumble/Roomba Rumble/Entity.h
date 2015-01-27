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

class Entity
{
private:
	vector<GLfloat> vertices, normals, texVertices;
	vector<GLuint> faceIndices, normIndices, texIndices;
protected:
	PhysicsManager* physicsManager;
	PxMaterial* material;
	PxRigidDynamic* hitbox;
	vec3 position;
	quat rotation;
public:
	Entity(PhysicsManager* physicsManager, vec3 position);
	~Entity();

	int readObj(char *filename);
	virtual void Update();
};

