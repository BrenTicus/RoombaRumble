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
public:
	Powerup(PhysicsManager* physicsManager, vec3 position, string filename);
	int getType() { return type; }
};

