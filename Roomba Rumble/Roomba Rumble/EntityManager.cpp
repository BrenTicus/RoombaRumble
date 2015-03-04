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

//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer, Entity* who, Entity* to){

	vec3 whoDir;
	whoDir.x = 1;
	whoDir.y = 0;
	whoDir.z = 1;


	whoDir = who->getRotation() * whoDir;
	vec3 toDir =  (to->getPosition() - who->getPosition());



	vec3 diff = who->getRotation() *(toDir - whoDir);
	toDir = glm::normalize(toDir);
	whoDir = glm::normalize(whoDir);
	diff = glm::normalize(diff);
	//printf("whoDir Z %f X %f\n", whoDir.z, whoDir.x);
	//printf("toDir Z %f X %f\n", toDir.z, toDir.x);
	//printf("diffDir Z %f X %f\n", diff.z, diff.x);
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

	if ((diff.x >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.x <= (1.0 * STEER_BUFFER_DELTA)) && (diff.z >=0.9f)){
		//printf("SDFKJLSDFKLJSDLKFJSLKFJ:LSKJDLk");
		buffer->steer = 0.0f;
	}
	else{
		buffer->steer = 0.5f * negation;
	}
	
	//buffer->steer = negation;
	buffer->accel = 0.35f;//accelApproach(getDistance(who->getPosition(), to->getPosition()));
	buffer->braking = 0.0;


}

void driveTo90(DriveControl* buffer, Entity* who, bool turnRight){
	
	vec3 whoDir;
	whoDir.x = 1;
	whoDir.y = 1;
	whoDir.z = 1;


	whoDir = who->getRotation() * whoDir;

	vec3 toDir;
	toDir.x = 1;
	toDir.y = 0;
	toDir.z = 0;



	vec3 diff = (toDir - whoDir);
	toDir = glm::normalize(toDir);
	//printf("Z %f\nX %f\n", toDir.z, toDir.x);

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

	whoDir = normalize(whoDir);
	toDir = normalize(toDir);
	
	//float dot = whoDir.x * toDir.x + whoDir.y * toDir.y + whoDir.z * toDir.z;
	float dot = length(diff);

	if ((toDir.x >= (-1.0f*STEER_BUFFER_DELTA)) && (toDir.x <= (1.0 * STEER_BUFFER_DELTA)) && (toDir.z >=0.0f)){
		//printf("SDFKJLSDFKLJSDLKFJSLKFJ:LSKJDLk Z %f\n X %f\n", toDir.z, toDir.x);
		buffer->steer = 0.0f;
	}
	else{
		buffer->steer = 0.5f * negation;
	}
	
	//buffer->steer = negation;
	buffer->accel = 0.5f;//accelApproach(getDistance(who->getPosition(), to->getPosition()));
	buffer->braking = 0.0;
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

					//printf("I SEE YOU \n");
					//printf("dist %f\n", entityDistance);

					driveTowards(aiControls[i], curAI, entityList[j]);
					//printf("Will steer %s\n", aiControls[i]->steer >=0 ? "RIGHT" : "LEFT");
				}
			}




		}
	}

}


