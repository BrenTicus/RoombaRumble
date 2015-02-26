#pragma once
#include "Entity.h"
#include "Powerup.h"

struct weapon {
	int type;
	obj* model;
	int damage;
	int level;
};


class Roomba : public Entity
{
protected:
	PxVehicleDrive4W* car;
	obj* wheel;
	obj* test;
	weapon* powerup;
	int health;
	int maxHealth;
	int vehicleIndex;
public:
	Roomba(PhysicsManager* physicsManager_, vec3 position_, string filename_);

	virtual int Update();
	virtual void Destroy();
	void addPowerup(int type);

	int getHealth() { return health; }
	void setHealth(int h) { health = std::max(h, maxHealth); }
	int doDamage(int d);
	int heal(int h);
};
