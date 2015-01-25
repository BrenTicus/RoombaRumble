#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;

	entityList.push_back(Entity(physicsManager, vec3(1.0f, 10.0f, 1.0f)));
}


EntityManager::~EntityManager()
{
}

void EntityManager::Update()
{
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		entityList[i].Update();
	}
}