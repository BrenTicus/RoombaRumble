#include "Roomba.h"


Roomba::Roomba(PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));
	wheel = (obj*)malloc(sizeof(obj));

	readObj(model, "Assets/roomba.obj");

	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);

	hitbox = physicsManager->addDynamicObject(physicsManager->physics->createShape(PxConvexMeshGeometry(mesh), *material), PxVec3(position.x, position.y, position.z), 1.0f);
}


Roomba::~Roomba()
{
}
