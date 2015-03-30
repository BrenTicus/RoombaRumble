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

	int pathIndex;			//path taking
	int pathNodeIndex;		//node

	int stuckCycle;
	int stuckCycleCount;
	vec3 lastPosition;


	//escaping stuckness save state variables
	vec3 revOldPosition;


	vec3 targetPos;
	Entity* targetEntity;

	void (AIRoomba::*stateFunc)(std::vector<Entity*>*);

	std::mt19937 randGen;			//somewhat hardcore RNG
	int getRandInt(int min, int max);
	bool getRandTrue(float chance);
	//vec3 getRandRoam();
	vec3 getNextRoamTarget();
	void setNearestRoamTarget();
	void initPathFind();

	//state functions
	void State_Roam(std::vector<Entity*>*);
	void State_Attack(std::vector<Entity*>*);
	void State_EscapeStuck(std::vector<Entity*>*);
	void State_GetItem(std::vector<Entity*>*);
	void State_Escape(std::vector<Entity*>*);

public:

	AIRoomba::AIRoomba(vec3 position_) : Roomba (position_)
	{
		static int globalID =0;
		id = globalID;
		globalID++;


		//setup paths variables and target
		initPathFind();


		//intailize instance variables
		cycle = INT_MAX;			//must update self on first run
		stuckCycle = 0;
		stateFunc = &AIRoomba::State_Roam;		//start in roam state
		

		
		

		lastPosition = vec3(INT_MAX,INT_MAX,INT_MAX);

		randGen.seed((unsigned long)std::chrono::system_clock::now().time_since_epoch().count());
		
	}


	AIRoomba::~AIRoomba();

	virtual int Update();


	int UpdateAI(std::vector<Entity*>*);
	

	void setTarget(Entity*);
	void setTarget(vec3);
	vec3 getTargetPos(void);


	virtual void Destroy();
	virtual void getControl() { return; };

	void AIRoomba::killAI(){ alive = false; }

	int getID(){return id;}
};