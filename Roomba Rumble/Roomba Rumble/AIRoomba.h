#pragma once
#include "Roomba.h"

class AIRoomba : public Roomba
{
private:
	bool alive;

public:

	AIRoomba::AIRoomba(PhysicsManager* physicsManager_, vec3 position_, string filename_) : Roomba (physicsManager_, position_, filename_){
		alive = true;
	}


	AIRoomba::~AIRoomba();

	virtual int Update();
	virtual void Destroy();

	void AIRoomba::killAI(){ alive = false; }


};