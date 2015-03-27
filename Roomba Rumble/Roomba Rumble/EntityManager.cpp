#include "EntityManager.h"
#include <iostream>
#include <string>
#include <glm/gtx/rotate_vector.hpp>


EntityManager::EntityManager(PhysicsManager* physicsManager)
{
	this->physicsManager = physicsManager;
	control = new Controller();

	RendererInfoFetcher rend("Utility/ObjectInfo.ini");
	rend.getEMInfo();

	int pIndex = 0;
	int cIndex = 0;
	for(int i = 0; i < rend.numObjs; i++)
	{	
		if(rend.types[i] == "roomba")
		{
			Roomba* newRoomba = new Roomba(physicsManager, control, cIndex++, rend.startPositions[i], rend.objFileNames[i]);
			newRoomba->setTag("roomba");
			newRoomba->setPowerupID("N/A");
			entityList.push_back(newRoomba);
			roombas.push_back(newRoomba);
		}
		else if(rend.types[i] == "airoomba")
		{
			AIRoomba* newAI = new AIRoomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newAI->setTag("airoomba");
			newAI->setPowerupID("N/A");
			entityList.push_back(newAI);
			aiRoombas.push_back(newAI);
			aiControls.push_back(new DriveControl());
		}
		else if(rend.types[i] == "powerup")
		{
			Powerup* newPowerup = new Powerup(physicsManager, rend.startPositions[i], rend.objFileNames[i], rend.powerupTypes[pIndex]);
			newPowerup->setTag("powerup");
			entityList.push_back(newPowerup);
			powerups.push_back(newPowerup);
			pIndex++;
		}
		else if(rend.types[i] == "static")
		{
			StaticObject* newStatic = new StaticObject(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newStatic->setTag("static");
			staticList.push_back(newStatic);
		}
	}
	rif = rend;

	projModel = new obj();
	entityList[0]->readObj(projModel, "Assets/projectile_1.obj");
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
	control->update();
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		ok = entityList[i]->Update();
		if (ok < 0) {
			entityList[i]->Destroy();
		}
		else if (ok > 0) {
			Projectile* proj = ((Roomba*)entityList[i])->createProjectile(projModel);
			proj->setTag("projectile");
			entityList.push_back(proj);
			sound->playSound("elastic.aiff"); //http://www.freesound.org/people/beskhu/sounds/149602/ 
		}
	}
}

void EntityManager::LateUpdate()
{
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		if (entityList[i]->isDestroyed()) {
			if (strcmp(entityList[i]->getTag(), "roomba") == 0)
			{
				for (unsigned int j = 0; j < roombas.size(); j++)
				{
					if (roombas[j]->isDestroyed()) roombas.erase(roombas.begin() + j--);
				}
			}
			// http://www.freesound.org/people/ryansnook/sounds/110113/
			if(strcmp(entityList[i]->getTag(), "airoomba") == 0)
				sound->playSound("medexplosion.wav");
			else if(strcmp(entityList[i]->getTag(), "roomba") == 0)
				sound->playSound("medexplosion.wav");

			entityList.erase(entityList.begin() + i--);
		}
	}
}


void EntityManager::respawnPowerups()
{
	for(unsigned int i = 0; i < powerups.size(); i++)
	{
		vec3 pPosition = powerups[i]->getPosition();

		if(!powerupActive(pPosition))
		{
			Powerup* newPowerup = new Powerup(physicsManager, powerups[i]->getPosition(), powerups[i]->getModel(), powerups[i]->getPowerupID());
			newPowerup->setTag("powerup");
			newPowerup->pIndex = i;
			entityList.push_back(newPowerup);
		}
	}
}

bool EntityManager::powerupActive(vec3 powerupPosition)
{
	for(unsigned int i = 0; i < entityList.size(); i++)
	{
		if(strcmp(entityList[i]->getTag(), "powerup") == 0)
		{
			vec3 ePosition = entityList[i]->getPosition();
			
			if(ePosition == powerupPosition)
				return true;
		}
	}

	return false;
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
		}
	}

}


