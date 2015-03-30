#include "StaticObject.h"

StaticObject::StaticObject()
{
}

StaticObject::StaticObject(vec3 position, string modelName)
{
	this->physicsManager = PhysicsManager::mainPhysicsManager;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.5f, 0.4f, 0.5f);
	model = (obj*)malloc(sizeof(obj));

	readObj(model, modelName);

	vector<PxU32> facelist;

	vector<PxVec3> vertexlist = objToVectors(model, &facelist);
	PxTriangleMesh* mesh = physicsManager->createTriangleMesh(&vertexlist[0], model->vertices->size() / 4, &facelist[0], model->faceIndices->size() / 3);

	physicsManager->addStaticObject(mesh, PxVec3(position.x, position.y, position.z));
}

StaticObject::~StaticObject()
{
}
