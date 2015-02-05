#include "StaticObject.h"

StaticObject::StaticObject(string modelName, PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));

	readObj(model, &modelName[0]);

	vector<PxU32> facelist;

	vector<PxVec3> vertexlist = objToVectors(model, &facelist);
	PxTriangleMesh* mesh = physicsManager->createTriangleMesh(&vertexlist[0], model->vertices->size() / 4, &facelist[0], model->faceIndices->size() / 3);

	physicsManager->addStaticObject(mesh, material, PxVec3(position.x, position.y, position.z));
}

StaticObject::~StaticObject()
{
}
