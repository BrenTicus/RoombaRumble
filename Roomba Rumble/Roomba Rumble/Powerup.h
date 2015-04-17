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

const float ROTATE_DEGREES = 1.0f;

class Powerup : public Entity
{
protected:
	int type;
	string modelFile;
public:
	Powerup(vec3 position, string powerupType);
	~Powerup();
	virtual int Update();

	int getType() { return type; }
	string getModelFile() { return modelFile; }
};

