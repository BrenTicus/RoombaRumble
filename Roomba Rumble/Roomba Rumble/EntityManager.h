#pragma once

#include <vector>
#include "Entity.h"
#include "Roomba.h"
#include "StaticObject.h"
#include "RendererInfoFetcher.h"

class EntityManager
{
private:
	PhysicsManager* physicsManager;
public:
	std::vector<Entity*> entityList;
	std::vector<Roomba*> aiRoombas;
	std::vector<Roomba*> playerRoombas;
	std::vector<StaticObject*> staticList;

	EntityManager();
	EntityManager(PhysicsManager* physicsManager, int, int);
	~EntityManager();

	void Update();


	vector<Roomba*>* getAIRoombas(void);
	vector<Roomba*>* getPlayerRoombas(void);
};

