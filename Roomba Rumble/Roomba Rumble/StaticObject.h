#pragma once
#include "Entity.h"
class StaticObject : public Entity
{
public:
	StaticObject(string modelName, PhysicsManager* physicsManager, vec3 position);
	~StaticObject();
};

