#include "Roomba.h"

Roomba::Roomba(PhysicsManager* physicsManager, vec3 position)
{
	// Initialize various variables.
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));
	wheel = (obj*)malloc(sizeof(obj));
	maxHealth = 5;
	health = maxHealth;

	// Read in the models
	readObj(model, "Assets/roomba.obj");
	readObj(wheel, "Assets/wheel.obj");

	// Create the physics stuff
	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);
	vertexlist = objToVectors(wheel);
	PxConvexMesh* wheelMesh = physicsManager->createConvexMesh(&vertexlist[0], wheel->vertices->size() / 4);
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.5, -0.05, 0.7), PxVec3(0.5, -0.05, 0.7), PxVec3(-0.5, -0.05, -0.7), PxVec3(0.5, -0.05, -0.7) };
	PxConvexMesh* wheelMeshes[4] = { wheelMesh, wheelMesh, wheelMesh, wheelMesh };

	hitbox = physicsManager->createVehicle(*material, 20.0f, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	actor = (ActorData*)malloc(sizeof(ActorData));
	actor->type = ROOMBA_ACTOR;
	actor->parent = this;
	hitbox->userData = actor;
}

Roomba::~Roomba()
{
}

void Roomba::Update()
{
	printf("CLASS ROOMBA");
}