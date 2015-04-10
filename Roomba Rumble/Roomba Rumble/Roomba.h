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

const float CHASSIS_MASS = 75.0f;

const int BASE_CHASSIS_DAMAGE = 1;
const int BASE_MELEE_DAMAGE = 2;
const int BASE_RANGE_DAMAGE = 1;
const int BASE_SHIELD_FORCE = 30;
const int BASE_MAX_HEALTH = 5;
const int SHIELD_HEALTH_BONUS = 2;
const int SHIELD_DAMAGE_REDUCTION = 1;
const int HEAL_AMOUNT = 2;
const float MAX_SHOT_COOLDOWN = 1.0f * CLOCKS_PER_SEC;
const float INVINCIBLE_COOLDOWN = 5.0f * CLOCKS_PER_SEC;
const float JUMP_COOLDOWN = 1.0f;

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
	int damageReduce;
	int vehicleIndex;
	int controllerIndex;
	PxShape* shapeToRemove;
	PxVec3* force;
	float lastPickupTime;
	float lastShotTime;
	float lastJumpTime;
	float invincibleTimer;
	bool invincibleMode;
	bool addPowerupShape, powerupAttached, powerupCooldown;
	int kills;
public:
	Roomba(vec3 position);
	Roomba(Controller* controller, int controllerIndex, vec3 position);

	virtual int Update();
	virtual void Destroy();
	virtual void getControl();
	void addPowerup(int type);
	void validatePowerup();
	void applyForce(PxVec3* force);
	class Projectile* createProjectile();

	void activate(glm::vec3 position);
	void deactivate();

	void decVehicleIndex() { vehicleIndex--; }
	bool isPowAttached() { return powerupAttached; }
	void powIsAttached(bool flag) { powerupAttached = flag; }
	int getHealth() { return health; }
	int getMaxHealth() { return maxHealth; }
	void setHealth(int h) { health = std::max(h, maxHealth); }
	int heal(int h);
	int doDamage(int d);
	int getDamage() { return powerup->damage; }
	void setPowerupFlag(bool b) { addPowerupShape = false; }
	bool getPowerupFlag() { return addPowerupShape; }
	bool hasPowerup(){ return !(powerup->type == NO_UPGRADE); }
	int getPowerupType() { return powerup->type; }
	int getPowerupLevel() { return powerup->level; }
	void incKills() { kills += 1; }
	int getKills() { return kills; }
};
