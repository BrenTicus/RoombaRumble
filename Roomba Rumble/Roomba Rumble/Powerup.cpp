#include "Powerup.h"


Powerup::Powerup(vec3 position, string powerupType)
{
	destroy = false;
	this->physicsManager = PhysicsManager::mainPhysicsManager;
	this->resourceManager = ResourceManager::mainResourceManager;
	this->position = position;
	powerupID = powerupType;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	hitbox = physicsManager->addTriggerObject(&PxSphereGeometry(1.0), PxVec3(position.x, position.y, position.z), 100.0f);
	hitbox->userData = new ActorData();
	((ActorData*)hitbox->userData)->type = POWERUP_ACTOR;
	physicsManager->setParent(this, hitbox); 

	model = new obj();
	if (powerupType == "melee")
	{
		type = MELEE_UPRADE;
		model = resourceManager->powerupMelee;
	}
	else if (powerupType == "ranged")
	{
		type = RANGED_UPGRADE;
		model = resourceManager->powerupRange;
	}
	else if (powerupType == "shield")
	{
		type = SHIELD_UPGRADE;
		model = resourceManager->powerupShield;
	}
	else if (powerupType == "health")
	{
		type = HEALTH_PICKUP;
		model = resourceManager->powerupHealth;
	}

	//cout << this << endl;
}
Powerup::Powerup(vec3 position, obj* model, string powerupType)
{
	destroy = false;
	this->physicsManager = PhysicsManager::mainPhysicsManager;
	this->position = position;
	powerupID = powerupType;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	hitbox = physicsManager->addTriggerObject(&PxSphereGeometry(1.0), PxVec3(position.x, position.y, position.z), 100.0f);
	hitbox->userData = new ActorData();
	((ActorData*)hitbox->userData)->type = POWERUP_ACTOR;
	physicsManager->setParent(this, hitbox); 

	model = new obj();
	if (powerupType == "melee")
	{
		type = MELEE_UPRADE;
	}
	else if (powerupType == "ranged")
	{
		type = RANGED_UPGRADE;
	}
	else if (powerupType == "shield")
	{
		type = SHIELD_UPGRADE;
	}
	else if (powerupType == "health")
	{
		type = HEALTH_PICKUP;
	}
	//readObj(model, filename);
	this->model = model;

	//justAdded = false;

	//cout << this << endl;
}
int Powerup::Update()
{
	if (destroy) {
		return -1;
	}
	else return 0;
}