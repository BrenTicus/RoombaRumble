#pragma once
#include "Entity.h"
#include "Powerup.h"

struct weapon {
	int type;
	int damage;
	int level;
	PxShape* shape;
};

const int BASE_MELEE_DAMAGE = 2;
const int BASE_RANGE_DAMAGE = 1;
const int BASE_SHIELD_FORCE = 20;

class Roomba : public Entity
{
protected:
	PxVehicleDrive4W* car;
	obj* wheel;
	weapon* powerup;
	int health;
	int maxHealth;
	int vehicleIndex;
	PxShape* shapeToRemove;
	PxVec3* force;
	float lastPickupTime;
	bool addPowerupShape, powerupAttached, powerupCooldown;
public:
	Roomba(PhysicsManager* physicsManager_, vec3 position_, string filename_);

	virtual int Update();
	virtual void Destroy();
	void addPowerup(int type);
	void validatePowerup();
	void applyForce(PxVec3* force);

	bool isPowAttached() { return powerupAttached; }
	void powIsAttached(bool flag) { powerupAttached = flag; }
	int getHealth() { return health; }
	void setHealth(int h) { health = std::max(h, maxHealth); }
	int doDamage(int d);
	int getDamage() { return powerup->damage; }
	void setPowerupFlag(bool b) { addPowerupShape = false; }
	bool getPowerupFlag() { return addPowerupShape; }
	int getPowerupType() { return powerup->type; }
	//obj* getPowModel() { return powerup->model; }
	int heal(int h);
};
