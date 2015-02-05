#pragma once
#include "Entity.h"
class Roomba : public Entity
{
protected:
	PxVehicleDrive4W* car;
	obj* wheel;
public:
	Roomba(PhysicsManager* physicsManager_, vec3 position_);
	~Roomba();

	void Update();
};

