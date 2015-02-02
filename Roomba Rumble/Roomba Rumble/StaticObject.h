#pragma once
#include "Entity.h"
class StaticObject : public Entity
{
public:
	StaticObject(PhysicsManager* physicsManager, vec3 position);
	~StaticObject();
};

