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


//given two position vectors, get the distance
static float getDistance(vec3 pos1, vec3 pos2){
	//printf("POS1 (%f, %f, %f)\n", pos1.x, pos1.y, pos1.z);
	//printf("POS2 (%f, %f, %f)\n", pos2.x, pos2.y, pos2.z);
	vec3 diff = pos1 - pos2;
	return glm::sqrt(diff.x*diff.x + diff.y* diff.y + diff.z* diff.z);
}


const float ACCEL_DISTANCE_CAP = 20.0f;

//returns acceleration between 0.0 - 1.0 as one approaches
static float accelApproach(float distance){
	float min = distance >= ACCEL_DISTANCE_CAP ? ACCEL_DISTANCE_CAP : distance;
	return min / ACCEL_DISTANCE_CAP;
}


static vec3 normalize(vec3 v){
	float length = glm::sqrt(v.x*v.x + v.y* v.y + v.z* v.z);
	vec3 newV;
	newV.x = v.x / length;
	newV.y = v.y /length;
	newV.z = v.z /length;
	return newV;
}

static float length(vec3 v){ return glm::sqrt(v.x*v.x + v.y* v.y + v.z* v.z);}


const float STEER_BUFFER_DELTA = 0.25f;

vec3 getForwardVector(quat q) 
{
    return vec3( 2 * (q.x * q.z + q.w * q.y), 
                    2 * (q.y * q.x - q.w * q.x),
                    1 - 2 * (q.x * q.x + q.y * q.y));
}

//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer, Entity* who, Entity* to){

	vec3 whoDir(1,1,1);
	whoDir.x = 1;
	whoDir.y = 1;
	whoDir.z = 1;


	//whoDir = glm::rotateY((who->getRotation() * whoDir), 0.0f);
	//whoDir.x = whoDir.x > 0 ? glm::min(whoDir.x, 1.0f): glm::max(whoDir.x, -1.0f);
	vec3 toDir =  (to->getPosition() - who->getPosition());


	whoDir = getForwardVector(who->getRotation());
	


	toDir = glm::normalize(toDir);
	whoDir = glm::normalize(whoDir);

	vec3 diff = (toDir - whoDir);


	printf("whoDir Z %f Y %f X %f\n", whoDir.z, whoDir.y, whoDir.x);
	printf("toDir Z %f Y %f X %f\n", toDir.z, toDir.y, toDir.x);
	printf("diffDir Z %f Y %f X %f\n", diff.z, diff.y, diff.x);
	float negation = ( toDir.x) > 0 ? 1.0 : -1.0;


	/*
	if (toDir.x <= (0-STEER_BUFFER_DELTA)){
		//turn right
		buffer->steer = 1.0f * negation;
	}
	else if (toDir.x > (0 + STEER_BUFFER_DELTA)){
		//turn left
		buffer->steer = 1.0f * negation;
	}
	else if (toDir.x <= 0){
		buffer->steer = 0;
	}
	else if (toDir.x > 0){
		buffer->steer = 0;
	}
	*/

	
	//float dot = whoDir.x * toDir.x + whoDir.y * toDir.y + whoDir.z * toDir.z;
	float dot = length(diff);

	if ((diff.z >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.z <= (1.0 * STEER_BUFFER_DELTA)) && (diff.x >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.x <= (1.0 * STEER_BUFFER_DELTA))){
		printf("DRIVE STRAIGHT");
		buffer->steer = 0.0f;
		//buffer->steer = 0.75f * negation;
	}
	else{
		buffer->steer = 0.75f * negation;
	}
	
	//buffer->steer = negation;
	buffer->accel = 0.35f;//accelApproach(getDistance(who->getPosition(), to->getPosition()));
	buffer->braking = 0.0;


}

void driveTo90(DriveControl* buffer, Entity* who, bool turnRight){

}



const float AWARE_DISTANCE = 390.0f;

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

					//printf("dist %f\n", entityDistance);

					driveTowards(aiControls[i], curAI, entityList[j]);
					printf("Will steer %s\n", aiControls[i]->steer >=0 ? "RIGHT" : "LEFT");
				}
			}




		}
	}

}


