#pragma once
#include "Entity.h"
#include "Powerup.h"
#include "Projectile.h"
#include "Controller.h"

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
	Controller* controller;
	DriveControl* control;
	PxVehicleDrive4W* car;
	obj* wheel;
	weapon* powerup;
	int health;
	int maxHealth;
	int vehicleIndex;
	int controllerIndex;
	PxShape* shapeToRemove;
	PxVec3* force;
	float lastPickupTime;
	bool addPowerupShape, powerupAttached, powerupCooldown;
public:
	Roomba(PhysicsManager* physicsManager, vec3 position, string filename);
	Roomba(PhysicsManager* physicsManager, Controller* controller, int controllerIndex, vec3 position, string filename);

	virtual int Update();
	virtual void Destroy();
	virtual void getControl();
	void addPowerup(int type);
	void validatePowerup();
	void applyForce(PxVec3* force);
	Projectile* createProjectile();

	void decVehicleIndex() { vehicleIndex--; }
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
