#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;

	RendererInfoFetcher rif("Utility/ObjectInfo.ini");

	entityList.push_back(new Roomba(physicsManager, rif.startPositions[0], rif.objFileNames[0]));
	entityList.push_back(new AIRoomba(physicsManager, rif.startPositions[1], rif.objFileNames[1]));
	entityList.push_back(new Powerup(physicsManager, rif.startPositions[2], rif.objFileNames[2]));
	staticList.push_back(new StaticObject(physicsManager, rif.startPositions[3], rif.objFileNames[3]));
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
