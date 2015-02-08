#pragma once
#include "Entity.h"
class StaticObject : public Entity
{
public:
	StaticObject();
	StaticObject(string modelName, PhysicsManager* physicsManager, vec3 position);
	~StaticObject();
};

