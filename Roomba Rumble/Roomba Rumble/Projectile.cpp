#include "Projectile.h"


Projectile::Projectile(PhysicsManager* physicsManager, vec3 position, vec3 direction, int damage, obj* proj)
{
	this->physicsManager = physicsManager;
	this->position = position;
	this->damage = damage;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = new obj();
	destroy = false;

	//readObj(model, "Assets/projectile_1.obj");
	model = proj;

	PxVec3 force = PxVec3(direction.x, direction.y, direction.z);

	hitbox = physicsManager->addDynamicObject(&PxSphereGeometry(0.2f), PxVec3(position.x, position.y, position.z), 1.0f);
	hitbox->addForce(force, PxForceMode::eIMPULSE);
	ActorData* data = new ActorData();
	data->type = PROJECTILE_ACTOR;
	data->parent = this;
	hitbox->userData = data;
	if (damage > 1) hitbox->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}


Projectile::~Projectile()
{
}
