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
}

Powerup::~Powerup()
{
	delete hitbox->userData;
}

int Powerup::Update()
{
	return destroy;
}