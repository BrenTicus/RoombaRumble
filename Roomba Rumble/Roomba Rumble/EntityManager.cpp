#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;

	entityList.push_back(new Roomba(physicsManager, vec3(1.0f, 3.0f, 1.0f)));
	entityList.push_back(new Roomba(physicsManager, vec3(5.0f, 3.0f, 5.0f)));
	entityList.push_back(new Powerup(physicsManager, vec3(10, 1, 5)));
	staticList.push_back(new StaticObject(string("Assets/Level0.obj"), physicsManager, vec3(0.0f, 0.0f, 0.0f)));
}


EntityManager::~EntityManager()
{
}

void EntityManager::Update()
{
	int ok;
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		 ok = entityList[i]->Update();
		 if (ok != 0) {
			 entityList[i]->Destroy();
			 entityList.erase(entityList.begin() + i);
		 }
	}
}
