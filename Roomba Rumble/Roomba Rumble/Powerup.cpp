#include "Powerup.h"


Powerup::Powerup(PhysicsManager* physicsManager, vec3 position, string filename)
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
	type = MELEE_UPRADE;
	switch (type){
		case MELEE_UPRADE: readObj(model, filename); break;
		case RANGED_UPGRADE: readObj(model, filename); break;
		case SHIELD_UPGRADE: readObj(model, filename); break;
		case HEALTH_PICKUP: readObj(model, filename); break;
		default: readObj(model, filename); break;
	}
}