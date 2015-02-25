#include "Roomba.h"

Roomba::Roomba(PhysicsManager* physicsManager, vec3 position)
{
	this->physicsManager = physicsManager;
	// Initialize various variables.
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));
	wheel = (obj*)malloc(sizeof(obj));
	test = (obj*)malloc(sizeof(obj));
	maxHealth = 5;
	health = maxHealth;

	// Read in the models
	readObj(model, "Assets/roomba.obj");
	readObj(wheel, "Assets/wheel.obj");
	readObj(test, "Assets/melee_1.obj");

	// Create the physics stuff
	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);
	vertexlist = objToVectors(wheel);
	PxConvexMesh* wheelMesh = physicsManager->createConvexMesh(&vertexlist[0], wheel->vertices->size() / 4);
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.5, -0.05, 0.7), PxVec3(0.5, -0.05, 0.7), PxVec3(-0.5, -0.05, -0.7), PxVec3(0.5, -0.05, -0.7) };
	PxConvexMesh* wheelMeshes[4] = { wheelMesh, wheelMesh, wheelMesh, wheelMesh };

	hitbox = physicsManager->createVehicle(*material, 20.0f, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	vehicleIndex = (int)((ActorData*)hitbox->userData)->parent;
	physicsManager->setParent(this, hitbox);

	vertexlist = objToVectors(test);
	PxConvexMesh* testmesh = physicsManager->createConvexMesh(&vertexlist[0], test->vertices->size() / 4);
	PxShape* blah = physicsManager->physics->createShape(PxConvexMeshGeometry(testmesh), *material);
	ActorData* actor = new ActorData();
	actor->type = WEAPON_SHAPE;
	actor->parent = this;
	physicsManager->addShape(blah, hitbox);
	blah->userData = actor;
	cout << this << endl;
}

void Roomba::Destroy()
{
	physicsManager->deleteVehicle(vehicleIndex);
	Entity::Destroy();
}

int Roomba::Update()
{
	return Entity::Update();
}

int Roomba::doDamage(int d)
{
	health = health - d; 

	if (health <= 0) destroy = true; 
	return health;
}

int Roomba::heal(int h)
{
	health += h; 
	health = std::max(health, maxHealth);
	return health;
}

void Roomba::addPowerup(int type)
{
	cout << "Powerup!" << this << endl;
}