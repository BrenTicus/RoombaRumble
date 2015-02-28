#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;

	RendererInfoFetcher rend("Utility/ObjectInfo.ini");
	rend.getEMInfo();

	int pIndex = 0;
	for(int i = 0; i < rend.numObjs; i++)
	{	
		if(rend.types[i] == "roomba")
		{
			entityList.push_back(new Roomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]));
		}
		else if(rend.types[i] == "airoomba")
		{
			entityList.push_back(new AIRoomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]));
		}
		else if(rend.types[i] == "powerup")
		{
			entityList.push_back(new Powerup(physicsManager, rend.startPositions[i], rend.objFileNames[i], rend.powerupTypes[pIndex]));
			pIndex++;
		}
		else if(rend.types[i] == "static")
		{
			staticList.push_back(new StaticObject(physicsManager, rend.startPositions[i], rend.objFileNames[i]));
		}
	}
	rend.clear();
	rif = rend;
}


EntityManager::~EntityManager()
{
}

RendererInfoFetcher EntityManager::getRif()
{
	return rif;
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
