#pragma once

#include <vector>
#include "Entity.h"
#include "Roomba.h"
#include "AIRoomba.h"
#include "StaticObject.h"

class EntityManager
{
private:
	PhysicsManager* physicsManager;
public:
	std::vector<Entity*> entityList;
	std::vector<StaticObject*> staticList;

	EntityManager();
	EntityManager(PhysicsManager* physicsManager);
	~EntityManager();

	void Update();
};

