#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;

	entityList.push_back(Roomba(physicsManager, vec3(1.0f, 3.0f, 1.0f)));
	staticList.push_back(StaticObject(string("Assets/Level0.obj"), physicsManager, vec3(0.0f, 0.0f, 0.0f)));
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
