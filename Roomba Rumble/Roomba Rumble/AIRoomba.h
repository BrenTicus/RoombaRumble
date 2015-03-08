#pragma once
#include "Roomba.h"
#include "PhysicsManager.h"
#include "Powerup.h"
#include <vector>
#include <chrono>
#include <random>

class AIRoomba : public Roomba
{
private:

	int id;
	bool alive;
	


	int cycle;
	
	char* action;

	vec3 targetPos;
	Entity* targetEntity;
	DriveControl control;

	std::mt19937 randGen;			//somewhat hardcore RNG
	int getRandInt(int min, int max);
	bool AIRoomba::getRandTrue(float chance);



	

public:

	AIRoomba::AIRoomba(PhysicsManager* physicsManager_, vec3 position_, string filename_) : Roomba (physicsManager_, position_, filename_){
		static int globalID;			//for unique object tagging of AI

		alive = true;
		//update global ID so that its unique for each AI roomba
		globalID = globalID % 1000;
		id = globalID;
		globalID++;



		//intailize instance variables
		cycle = INT_MAX;			//must update self on first run
		action = "roam";

		control.accel = 0;
		control.steer = 0;
		control.braking = 0;
		control.reversing = 0;

		randGen.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}


	AIRoomba::~AIRoomba();

	virtual int Update();
	int UpdateAI(std::vector<Entity*>*);
	virtual void Destroy();

	void AIRoomba::killAI(){ alive = false; }
	DriveControl* getControl() {return &control;}
	int getID(){ return id;}
};