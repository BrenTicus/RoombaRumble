#include "EntityManager.h"
#include <iostream>
#include <string>

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
			AIRoomba* newAI = new AIRoomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			entityList.push_back(newAI);
			aiRoombas.push_back(newAI);
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



const float AWARE_DISTANCE = 3.0f;

static float getDistance(vec3 pos1, vec3 pos2){
	vec3 diff = pos2 - pos1;
	return glm::sqrt((diff.x*diff.x) + (diff.y*diff.y) + (diff.z*diff.z));
}

void EntityManager::runAI(){
	

	AIRoomba* curAI;
	
	for (int i =0; i < aiRoombas.size() ; i++){
		curAI = aiRoombas[i];

		for (int j = 0; j < entityList.size(); j++){
	
			float entityDistance = getDistance(curAI->getPosition(), entityList[j]->getPosition());

			if (entityDistance <= AWARE_DISTANCE){
				//within field of chase
				if (strcmp(typeid(entityList[j]).name(), "AIRoomba") == 0){
					//printf("NEAR AI ");
				}
				//printf("dist %s\n", typeid(entityList[j]).name());
			}
			//printf("dist %f\n", entityDistance);
		}
	}

}






