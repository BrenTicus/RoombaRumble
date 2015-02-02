#pragma once

#include <vector>
#include "Entity.h"
#include "Roomba.h"
#include "StaticObject.h"

class EntityManager
{
private:
	PhysicsManager* physicsManager;
public:
	std::vector<Entity> entityList;
	std::vector<StaticObject> staticList;

	EntityManager(PhysicsManager* physicsManager);
	~EntityManager();

	void Update();
};

