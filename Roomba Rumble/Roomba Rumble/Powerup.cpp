#include "Powerup.h"


Powerup::Powerup(PhysicsManager* physicsManager, vec3 position)
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
	type = std::rand() % 3 + 1;	//Randomly pick an upgrade.
	switch (type){
		case MELEE_UPRADE: readObj(model, "Assets/melee_1.obj"); break;
		case RANGED_UPGRADE: readObj(model, "Assets/melee_1.obj"); break;
		case SHIELD_UPGRADE: readObj(model, "Assets/melee_1.obj"); break;
		case HEALTH_PICKUP: readObj(model, "Assets/melee_1.obj"); break;
		default: readObj(model, "Assets/melee_1.obj"); break;
	}
}