#include "EntityManager.h"
#include <iostream>
#include <string>
#include <glm/gtx/rotate_vector.hpp>


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
			Roomba* newRoomba = new Roomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newRoomba->setTag("roomba");
			entityList.push_back(newRoomba);
			roombas.push_back(newRoomba);
		}
		else if(rend.types[i] == "airoomba")
		{
			AIRoomba* newAI = new AIRoomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newAI->setTag("airoomba");
			entityList.push_back(newAI);
			aiRoombas.push_back(newAI);
			aiControls.push_back(new DriveControl());
		}
		else if(rend.types[i] == "powerup")
		{
			Powerup* newPowerup = new Powerup(physicsManager, rend.startPositions[i], rend.objFileNames[i], rend.powerupTypes[pIndex]);
			newPowerup->setTag("powerup");
			entityList.push_back(newPowerup);
			pIndex++;
		}
		else if(rend.types[i] == "static")
		{
			StaticObject* newStatic = new StaticObject(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newStatic->setTag("static");
			staticList.push_back(newStatic);
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
		}
	}
	/*for (unsigned int i = 0; i < roombas.size(); i++)
	{
		ok = roombas[i]->Update();
		if (ok != 0){
			roombas[i]->Destroy();
		}
	}*/
}

void EntityManager::LateUpdate()
{
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		if (entityList[i]->isDestroyed()) entityList.erase(entityList.begin() + i);
	}
}


void EntityManager::UpdateAI(){

	for (unsigned int i =0; i < aiRoombas.size() ; i++){
		if (aiRoombas[i]->isDestroyed())
		{
			aiRoombas.erase(aiRoombas.begin() + i);
			i--;
			continue;
		}
		else {
			aiRoombas[i]->UpdateAI(&entityList);
			aiControls[i]= aiRoombas[i]->getControl();
		}

		
	}

}


