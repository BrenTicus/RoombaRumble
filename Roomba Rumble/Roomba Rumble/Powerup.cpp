#include "Powerup.h"


Powerup::Powerup(PhysicsManager* physicsManager, vec3 position, string filename, string powerupType)
{
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	hitbox = physicsManager->addDynamicObject(&PxSphereGeometry(1.0), PxVec3(position.x, position.y, position.z), 100.0f);
	hitbox->userData = new ActorData();
	((ActorData*)hitbox->userData)->type = POWERUP_ACTOR;
	physicsManager->setParent(this, hitbox);

	model = (obj*)malloc(sizeof(obj));
	if (powerupType == "melee")
	{
		cout << "Melee powerup" << endl;
		type = MELEE_UPRADE;
	}
	else if (powerupType == "ranged")
	{
		cout << "Ranged powerup" << endl;
		type = RANGED_UPGRADE;
	}
	else if (powerupType == "shield")
	{
		cout << "Shield powerup" << endl;
		type = SHIELD_UPGRADE;
	}
	else if (powerupType == "health")
	{
		cout << "Health pickup" << endl;
		type = HEALTH_PICKUP;
	}
	readObj(model, filename);

	cout << this << endl;
}