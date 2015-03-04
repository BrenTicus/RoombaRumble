#include "Roomba.h"

Roomba::Roomba(PhysicsManager* physicsManager, vec3 position, string filename)
{
	this->physicsManager = physicsManager;
	// Initialize various variables.
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = new obj();
	wheel = new obj();
	maxHealth = 5;
	health = maxHealth;
	addPowerupShape = false;

	// Read in the models
	readObj(model, filename);
	readObj(wheel, string("Assets/wheel.obj"));

	// Create the physics stuff
	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);
	vertexlist = objToVectors(wheel);
	PxConvexMesh* wheelMesh = physicsManager->createConvexMesh(&vertexlist[0], wheel->vertices->size() / 4);
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.5f, -0.05f, 0.7f), PxVec3(0.5f, -0.05f, 0.7f), PxVec3(-0.5f, -0.05f, -0.7f), PxVec3(0.5f, -0.05f, -0.7f) };
	PxConvexMesh* wheelMeshes[4] = { wheelMesh, wheelMesh, wheelMesh, wheelMesh };

	hitbox = physicsManager->createVehicle(*material, 20.0f, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	vehicleIndex = (int)((ActorData*)hitbox->userData)->parent;
	physicsManager->setParent(this, hitbox);

	// Initialize the weapon.
	powerup = new weapon();
	powerup->damage = 1;
	powerup->level = 0;
	powerup->model = new obj();
	powerup->type = NO_UPGRADE;
}

void Roomba::Destroy()
{
	physicsManager->deleteVehicle(vehicleIndex);
	Entity::Destroy();
}

int Roomba::Update()
{
	if (addPowerupShape) {
		physicsManager->addShape(powerup->shape, hitbox);
		addPowerupShape = false;
	}
	return Entity::Update();
}

int Roomba::doDamage(int d)
{
	health = health - d; 

	if (health <= 0) {
		destroyFlag();
	}
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
	if (type == powerup->type)
	{
		powerup->level++;
	}
	else
	{
		powerup->type = type;
		powerup->level = 1;
	}
	validatePowerup();
}

void Roomba::validatePowerup()
{
	if (powerup->level > 3) { powerup->level = 3; return; }
	string filename;
	switch (powerup->type)
	{
	case MELEE_UPRADE: 
		filename = "Assets/melee_"; 
		powerup->damage = 2 * powerup->level;
		break;
	case RANGED_UPGRADE: 
		filename = "Assets/range_";
		powerup->damage = powerup->level;
		break;
	case SHIELD_UPGRADE: 
		filename = "Assets/shield_"; 
		powerup->damage = 1;
		break;
	}
	filename += std::to_string(powerup->level);
	filename += ".obj";
	readObj(powerup->model, (char*)filename.c_str());

	vector<PxVec3> vertices = objToVectors(powerup->model);
	//PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertices[0], vertices.size() / 4);
	if (powerup->shape) physicsManager->removeShape(powerup->shape, hitbox);

	//powerup->shape = physicsManager->physics->createShape(PxConvexMeshGeometry(mesh), *material);
	powerup->shape = physicsManager->physics->createShape(PxBoxGeometry(0.5, 0.1, 0.1), *material);
	powerup->shape->setLocalPose(PxTransform(PxVec3(0, 0.5, 1.0)));

	ActorData* data = new ActorData();
	data->type = WEAPON_SHAPE;
	data->parent = this;
	powerup->shape->userData = data;

	addPowerupShape = true;
}