#pragma once

#include <vector>
#include "Entity.h"
#include "Roomba.h"
#include "AIRoomba.h"
#include "StaticObject.h"
#include "RendererInfoFetcher.h"


class EntityManager
{
private:
	PhysicsManager* physicsManager;
	RendererInfoFetcher rif;

	//std::vector<DriveControl*> resultAIControls;
public:
	std::vector<Entity*> entityList;
	std::vector<AIRoomba*> aiRoombas;
	std::vector<StaticObject*> staticList;


	EntityManager();
	EntityManager(PhysicsManager* physicsManager);
	~EntityManager();

	RendererInfoFetcher getRif();

	void Update();
	void LateUpdate();

	void runAI();
};

