#pragma once
#include "Entity.h"
#include <random>

enum {
	NO_UPGRADE = 0,
	MELEE_UPRADE,
	RANGED_UPGRADE,
	SHIELD_UPGRADE,
	HEALTH_PICKUP
};

class Powerup : public Entity
{
protected:
	int type;
	string modelFile;
public:
	Powerup(PhysicsManager* physicsManager, vec3 position, string filename, string powerupType);
	virtual int Update();

	int getType() { return type; }
	string getModelFile() { return modelFile; }
};

