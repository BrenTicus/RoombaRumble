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
			Roomba* newRoomba = new Roomba(physicsManager, rend.startPositions[i], rend.objFileNames[i]);
			newRoomba->setTag("roomba");
			entityList.push_back(newRoomba);
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
}

void EntityManager::LateUpdate()
{
	for (unsigned int i = 0; i < entityList.size(); i++)
	{
		if (entityList[i]->isDestroyed()) entityList.erase(entityList.begin() + i);
	}
}



const float AWARE_DISTANCE = 30.0f;
const float STEER_BUFFER_DELTA = 0.2f;

static float getDistance(vec3 pos1, vec3 pos2){
	//printf("POS1 (%f, %f, %f)\n", pos1.x, pos1.y, pos1.z);
	//printf("POS2 (%f, %f, %f)\n", pos2.x, pos2.y, pos2.z);
	vec3 diff = pos1 - pos2;
	return glm::sqrt(diff.x*diff.x + diff.y* diff.y + diff.z* diff.z);
}


//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer, Entity* who, Entity* to){

	vec3 whoDir;
	whoDir.x = 1;
	whoDir.y = 1;
	whoDir.z = 1;


	whoDir = who->getRotation() * whoDir;
	vec3 toDir = to->getPosition() - who->getPosition();

	//whoDir = glm::normalize(whoDir);
	//toDir = glm::normalize(toDir);

	vec3 diff = (toDir - whoDir);
	diff = glm::normalize(diff);
	printf("Z %f\n X %f\n", diff.z, diff.x);

	float negation = (diff.z * diff.x) > 0 ? -1.0 : 1.0;


	
	if (diff.z <= (0-STEER_BUFFER_DELTA)){
	//turn right
	buffer->steer = 1.0f * negation;
	}
	else if (diff.z > (0 + STEER_BUFFER_DELTA)){
	//turn left
	buffer->steer = 1.0 * negation;
	}
	else if (diff.z <= 0){
	buffer->steer = 0;
	}
	else if (diff.z > 0){
	buffer->steer = 0;
	}
	
	
	buffer->accel = 1.0;
	buffer->braking = 0.0;


}


void EntityManager::UpdateAI(){


	AIRoomba* curAI;

	for (unsigned int i =0; i < aiRoombas.size() ; i++){
		if (aiRoombas[i]->isDestroyed())
		{
			aiRoombas.erase(aiRoombas.begin() + i);
			i--;
			continue;
		}
		curAI = aiRoombas[i];

		for (unsigned int j = 0; j < entityList.size(); j++){



			if ((strcmp(entityList[j]->getTag(), "roomba") == 0)){// || (strcmp(entityList[j]->getTag(), "airoomba") == 0)){
				//if not self
				float entityDistance = getDistance(curAI->getPosition(), entityList[j]->getPosition());

				if (entityDistance <= AWARE_DISTANCE){
					//within field of chase

					//printf("I SEE YOU \n");
					printf("dist %f\n", entityDistance);

					driveTowards(aiControls[i], curAI, entityList[j]);
					printf("Will steer %s\n", aiControls[i]->steer >=0 ? "RIGHT" : "LEFT");
				}
			}




		}
	}

}


