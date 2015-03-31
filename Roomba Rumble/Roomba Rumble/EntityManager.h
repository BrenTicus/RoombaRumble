#pragma once

#include <vector>
#include "ResourceManager.h"
#include "Controller.h"
#include "Entity.h"
#include "Roomba.h"
#include "AIRoomba.h"
#include "StaticObject.h"
#include "RendererInfoFetcher.h"
#include "Sound.h"
#include <random>
#include <chrono>

const float POWERUP_RESPAWN_COOLDOWN = 5.0f * CLOCKS_PER_SEC;
const float SPAWN_AI_COOLDOWN = 5.0f * CLOCKS_PER_SEC;
const float AI_UPDATE_COOLDOWN = 0.0166f * CLOCKS_PER_SEC;


class EntityManager
{
private:
	Controller* control;
	ResourceManager* resourceManager;
	RendererInfoFetcher rif;
	std::mt19937 randGen;

	int getRandInt(int,int);
public:
	static EntityManager* mainEntityManager;

	std::vector<Entity*> entityList;
	
	std::vector<StaticObject*> staticList;

	std::vector<Roomba*> roombas;
	std::vector<AIRoomba*> aiRoombas;
	std::vector<Powerup*> powerups;
	std::vector<DriveControl*> aiControls;

	Sound* sound;

	EntityManager();
	~EntityManager();

	RendererInfoFetcher getRif();

	void Update();
	void LateUpdate();
	void respawnPowerups();
	void spawnAIRandom();
	bool powerupActive(vec3 powerupPosition);

	void UpdateAI();
	int getAICount(){ return aiRoombas.size(); }
	int getPlayerCount() { return roombas.size();} 
	vec3 nearestRoomba(vec3 location);
	std::vector<DriveControl*>* getAIControls(){ return &aiControls;}

	void printPlayerLocation(){ 

		if (roombas.size() > 0){
			vec3 pos = roombas[0]->getPosition();
			vec3 speed = roombas[0]->getSpeed();
			printf("P1 Pos: X=%f Y=%f Z=%f, Speed: %f\n", pos.x, pos.y, pos.z, glm::length(speed));
		}
	}
};
