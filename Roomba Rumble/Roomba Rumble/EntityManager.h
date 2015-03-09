#pragma once

#include <vector>
#include "Controller.h"
#include "Entity.h"
#include "Roomba.h"
#include "AIRoomba.h"
#include "StaticObject.h"
#include "RendererInfoFetcher.h"


class EntityManager
{
private:
	PhysicsManager* physicsManager;
	Controller* control;
	RendererInfoFetcher rif;

public:
	std::vector<Entity*> entityList;
	
	std::vector<StaticObject*> staticList;

	std::vector<Roomba*> roombas;
	std::vector<AIRoomba*> aiRoombas;
	std::vector<DriveControl*> aiControls;

	EntityManager();
	EntityManager(PhysicsManager* physicsManager);
	~EntityManager();

	RendererInfoFetcher getRif();

	void Update();
	void LateUpdate();

	void UpdateAI();
	int getAICount(){ return aiRoombas.size(); }
	int getPlayerCount() { return roombas.size();} 
	std::vector<DriveControl*>* getAIControls(){ return &aiControls;}

	void printPlayerLocation(){ 

		if (roombas.size() > 0){
			vec3 pos = roombas[0]->getPosition();
			printf("P1 Pos: X=%f Y=%f Z=%f\n", pos.x, pos.y, pos.z);
		}
	}
};

