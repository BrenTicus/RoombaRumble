#include "Entity.h"
#include <iostream>


Entity::Entity(PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);

	hitbox = physicsManager->addDynamicObject(physicsManager->physics->createShape(PxCapsuleGeometry(0.5f, 1.0f), *material), PxVec3(position.x, position.y, position.z), 1.0f);
}


Entity::~Entity()
{
}

void Entity::Update()
{
	PxTransform pxtransform = hitbox->getGlobalPose();
	position = vec3(pxtransform.p.x, pxtransform.p.y, pxtransform.p.z);
	rotation = quat(pxtransform.q.w, pxtransform.q.x, pxtransform.q.y, pxtransform.q.z);

	//std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
}
