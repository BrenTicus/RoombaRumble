#include "EntityManager.h"


EntityManager::EntityManager(PhysicsManager* physicsManager, int playerCount, int botCount)
{
	this->physicsManager = physicsManager;

	RendererInfoFetcher rif("Utility/ObjectInfo.ini");

	
	Roomba* player = new Roomba(physicsManager, rif.startPositions[0], rif.objFileNames[0], false);
	entityList.push_back(player);
	Roomba* ai = new Roomba(physicsManager, rif.startPositions[1], rif.objFileNames[1], true);
	entityList.push_back(ai);
	entityList.push_back(new Powerup(physicsManager, rif.startPositions[2], rif.objFileNames[2]));
	staticList.push_back(new StaticObject(physicsManager, rif.startPositions[3], rif.objFileNames[3]));

	aiRoombas.push_back(ai);
	playerRoombas.push_back(player);
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


vector<Roomba*>* EntityManager::getAIRoombas(){
	return &aiRoombas;
}

vector<Roomba*>* EntityManager::getPlayerRoombas(){
	return &playerRoombas;
}