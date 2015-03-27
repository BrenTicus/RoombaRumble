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


	int stuckCycle;
	int stuckCycleCount;
	vec3 lastPosition;

	bool reverseLeft;			//for attempting to reverse
	vec3 revOldPosition;
	char* action;

	vec3 targetPos;
	Entity* targetEntity;

	void (AIRoomba::*stateFunc)(std::vector<Entity*>*);

	std::mt19937 randGen;			//somewhat hardcore RNG
	int getRandInt(int min, int max);
	bool getRandTrue(float chance);
	vec3 getRandRoam();


	//state functions
	void State_Roam(std::vector<Entity*>*);
	void State_Attack(std::vector<Entity*>*);
	void State_EscapeStuck(std::vector<Entity*>*);
	

public:

	AIRoomba::AIRoomba(PhysicsManager* physicsManager_, vec3 position_, string filename_) : Roomba (physicsManager_, position_, filename_)
	{
		static int globalID =0;
		id = globalID;
		globalID++;

		//intailize instance variables
		cycle = INT_MAX;			//must update self on first run
		stuckCycle = 0;
		action = "roam";
		stateFunc = &AIRoomba::State_Roam;		//start in roam state
		targetEntity = NULL;
		targetPos = getRandRoam();


		lastPosition = vec3(INT_MAX,INT_MAX,INT_MAX);

		randGen.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}


	AIRoomba::~AIRoomba();

	virtual int Update();


	int UpdateAI(std::vector<Entity*>*);





	virtual void Destroy();
	virtual void getControl() { return; };

	void AIRoomba::killAI(){ alive = false; }

	int getID(){return id;}
};