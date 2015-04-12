#include "Projectile.h"


Projectile::Projectile(PhysicsManager* physicsManager, vec3 position, vec3 direction, PxQuat rotation, int damage, Roomba* roomba)
{
	this->physicsManager = physicsManager;
	this->position = position;
	this->damage = damage;
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	destroy = false;

	PxVec3 force = PxVec3(direction.x, direction.y, direction.z);

	hitbox = physicsManager->addDynamicObject(&PxSphereGeometry(0.2f), PxVec3(position.x, position.y, position.z), 1.0f);
	hitbox->addForce(force, PxForceMode::eIMPULSE);
	ActorData* data = new ActorData();
	data->type = PROJECTILE_ACTOR;
	data->parent = this;
	hitbox->userData = data;
	
	hitbox->setGlobalPose(PxTransform(position.x, position.y, position.z, rotation));

	if (damage > 1) hitbox->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	originRoomba = roomba;
}


Projectile::~Projectile()
{
	delete hitbox->userData;
}
