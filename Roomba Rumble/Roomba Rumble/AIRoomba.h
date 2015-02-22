#pragma once
#include "Roomba.h"

class AIRoomba : public Roomba
{
private:
	bool alive;

public:

	AIRoomba::AIRoomba(PhysicsManager* physicsManager_, vec3 position_) : Roomba (physicsManager_, position_){
		alive = true;
	}


	AIRoomba::~AIRoomba();

	virtual void Update();

	void AIRoomba::killAI(){ alive = false; }


};