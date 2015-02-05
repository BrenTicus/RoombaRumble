#include "Roomba.h"


Roomba::Roomba(PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));
	wheel = (obj*)malloc(sizeof(obj));

	readObj(model, "Assets/roomba.obj");
	readObj(wheel, "Assets/wheel.obj");

	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);
	vertexlist = objToVectors(wheel);
	PxConvexMesh* wheelMesh = physicsManager->createConvexMesh(&vertexlist[0], wheel->vertices->size() / 4);

	//hitbox = physicsManager->addDynamicObject(physicsManager->physics->createShape(PxConvexMeshGeometry(mesh), *material), PxVec3(position.x, position.y, position.z), 1.0f);
	//physicsManager->addDynamicObject(physicsManager->physics->createShape(PxConvexMeshGeometry(wheelMesh), *material), PxVec3(position.x, position.y, position.z), 1.0f);
	//physicsManager->addDynamicObject(physicsManager->physics->createShape(PxConvexMeshGeometry(wheelMesh), *material), PxVec3(position.x+1, position.y-1, position.z), 1.0f);
	//physicsManager->addDynamicObject(physicsManager->physics->createShape(PxConvexMeshGeometry(wheelMesh), *material), PxVec3(position.x, position.y-1, position.z), 1.0f);
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.3, -0.5, 0.3), PxVec3(0.3, -0.5, 0.3), PxVec3(-0.3, -0.5, -0.3), PxVec3(0.3, -0.5, -0.3) };
	PxConvexMesh* wheelMeshes[4] = { wheelMesh, wheelMesh, wheelMesh, wheelMesh };

	hitbox = physicsManager->createVehicle(*material, 1.0f, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	assert(hitbox);
}


Roomba::~Roomba()
{
}

void Roomba::Update()
{

}