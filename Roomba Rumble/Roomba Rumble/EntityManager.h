#pragma once

#include <vector>
#include "Entity.h"

class EntityManager
{
private:
	PhysicsManager* physicsManager;
public:
	std::vector<Entity> entityList;

	EntityManager(PhysicsManager* physicsManager);
	~EntityManager();

	void Update();
};

