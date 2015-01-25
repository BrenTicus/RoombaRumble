#pragma once
#include "PhysicsManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"

using namespace glm;

class Entity
{
protected:
	PhysicsManager* physicsManager;
	PxMaterial* material;
	PxRigidDynamic* hitbox;
	vec3 position;
	quat rotation;

public:
	Entity(PhysicsManager* physicsManager, vec3 position);
	~Entity();

	virtual void Update();
};

