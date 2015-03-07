#pragma once
#include "Roomba.h"
#include "PhysicsManager.h"
#include <vector>

class AIRoomba : public Roomba
{
private:

	int id;
	bool alive;
	


	int cycle;
	
	char* action;

	DriveControl control;

public:

	AIRoomba::AIRoomba(PhysicsManager* physicsManager_, vec3 position_, string filename_) : Roomba (physicsManager_, position_, filename_){
		alive = true;
	}


	AIRoomba::~AIRoomba();

	virtual int Update();
	int UpdateAI(std::vector<Entity*>*);
	virtual void Destroy();

	void AIRoomba::killAI(){ alive = false; }
	DriveControl* getControl() {return &control;}

};