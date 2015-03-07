#include "Roomba.h"

Roomba::Roomba(PhysicsManager* physicsManager, vec3 position, string filename)
{
	this->physicsManager = physicsManager;
	// Initialize various variables.
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.4f);
	model = new obj();
	wheel = new obj();
	maxHealth = 5;
	health = maxHealth;
	addPowerupShape = false;
	force = new PxVec3(0,0,0);
	powerupCooldown = false;

	// Read in the models
	readObj(model, filename);
	readObj(wheel, string("Assets/wheel.obj"));

	// Create the physics stuff
	vector<PxVec3> vertexlist = objToVectors(model);
	PxConvexMesh* mesh = physicsManager->createConvexMesh(&vertexlist[0], model->vertices->size() / 4);
	vertexlist = objToVectors(wheel);
	PxConvexMesh* wheelMesh = physicsManager->createConvexMesh(&vertexlist[0], wheel->vertices->size() / 4);
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.5f, 0.03f, 0.7f), PxVec3(0.5f, 0.03f, 0.7f), PxVec3(-0.5f, 0.03f, -0.6f), PxVec3(0.5f, 0.03f, -0.6f) };
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

	powerupAttached = false;
}

void Roomba::Destroy()
{
	physicsManager->deleteVehicle(vehicleIndex);
	Entity::Destroy();
}

int Roomba::Update()
{
	if (addPowerupShape) {
		if (shapeToRemove != NULL)
		{
			physicsManager->removeShape(shapeToRemove, hitbox);
			shapeToRemove = NULL;
		}
		if (powerup->shape != NULL) physicsManager->addShape(powerup->shape, hitbox);
		addPowerupShape = false;
	}
	if (powerupCooldown) {
		if ((clock() - lastPickupTime) / CLOCKS_PER_SEC > 0.1f) powerupCooldown = false;
	}
	hitbox->addForce(*force, PxForceMode::eIMPULSE);
	force = new PxVec3(0,0,0);
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
	if (!powerupCooldown)
	{
		if (type == powerup->type)
		{
			powerup->level++;
		}
		else if (type == HEALTH_PICKUP)
		{
			heal(2);
			return;
		}
		else
		{
			powerup->type = type;
			powerup->level = 1;
		}
		validatePowerup();
		powerupCooldown = true;
		lastPickupTime = clock();
	}
}

void Roomba::validatePowerup()
{
	if (powerup->level > 3) { powerup->level = 3; return; }
	ActorData* data = new ActorData();
	switch (powerup->type)
	{
	case MELEE_UPRADE: 
		powerup->damage = BASE_MELEE_DAMAGE * powerup->level;

		shapeToRemove = powerup->shape;
		powerup->shape = physicsManager->physics->createShape(PxBoxGeometry(0.2 * powerup->level, 0.1, 0.3), *material);
		powerup->shape->setLocalPose(PxTransform(PxVec3(0, 0.3, 1.0)));
		data->type = WEAPON_SHAPE;
		data->parent = this;
		powerup->shape->userData = data;
		break;
	case RANGED_UPGRADE: 
		powerup->damage = BASE_RANGE_DAMAGE * powerup->level;
		shapeToRemove = powerup->shape;
		powerup->shape = NULL;
		break;
	case SHIELD_UPGRADE: 
		powerup->damage = BASE_SHIELD_FORCE * powerup->level;	// For the shield, this affects the bounce effect, not actual damage
		shapeToRemove = powerup->shape;
		powerup->shape = NULL;
		break;
	}

	addPowerupShape = true;
}

void Roomba::applyForce(PxVec3* force)
{
	this->force->x += force->x;
	this->force->y += force->y;
	this->force->z += force->z;
}