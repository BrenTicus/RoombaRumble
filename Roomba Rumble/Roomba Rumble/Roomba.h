#pragma once
#include "Entity.h"
class Roomba : public Entity
{
protected:
	obj* wheel;
public:
	Roomba(PhysicsManager* physicsManager_, vec3 position_);
	~Roomba();
};

