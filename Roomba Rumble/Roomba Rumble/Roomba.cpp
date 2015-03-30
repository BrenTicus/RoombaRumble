#include "Roomba.h"

template <class T>
T mymax(T a, T b)
{
	return a < b ? b : a;
}

Roomba::Roomba(vec3 position)
{
	this->physicsManager = PhysicsManager::mainPhysicsManager;
	this->resourceManager = ResourceManager::mainResourceManager;
	// Initialize various variables.
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.4f);
	maxHealth = BASE_MAX_HEALTH;
	health = maxHealth;
	damageReduce = 0;
	addPowerupShape = false;
	force = new PxVec3(0,0,0);
	powerupCooldown = false;
	control = new DriveControl();

	// Read in the models
	model = resourceManager->roomba;
	wheel = resourceManager->wheel;

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
	powerup->damage = BASE_CHASSIS_DAMAGE;
	powerup->level = 0;
	//powerup->model = new obj();
	powerup->type = NO_UPGRADE;

	powerupAttached = false;
}

Roomba::Roomba(Controller* controller, int controllerIndex, vec3 position)
{
	this->controller = controller;
	this->controllerIndex = controllerIndex;

	this->physicsManager = PhysicsManager::mainPhysicsManager;
	this->resourceManager = ResourceManager::mainResourceManager;
	// Initialize various variables.
	destroy = false;
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.4f);
	maxHealth = 5;
	health = maxHealth;
	damageReduce = 0;
	addPowerupShape = false;
	force = new PxVec3(0, 0, 0);
	powerupCooldown = false;
	control = new DriveControl();

	// Read in the models
	model = resourceManager->roomba;
	wheel = resourceManager->wheel;

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
	//powerup->model = new obj();
	powerup->type = NO_UPGRADE;

	powerupAttached = false;
}

void Roomba::Destroy()
{
	physicsManager->deleteVehicle(vehicleIndex);
	delete control;
	delete powerup;
	Entity::Destroy();
}

int Roomba::Update()
{
	getControl();
	physicsManager->inputControls(vehicleIndex, control);
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
	if (powerup->type == RANGED_UPGRADE)
	{
		if (!destroy && control->shooting && clock() - lastShotTime > MAX_SHOT_COOLDOWN)
		{
			lastShotTime = (float)clock();
			return 1;
		}
	}
	return Entity::Update();
}

int Roomba::doDamage(int d)
{
	health = health - (d - damageReduce); 

	if (health <= 0) {
		destroyFlag();
	}
	return health;
}

int Roomba::heal(int h)
{
	health += h; 
	health = std::min(health, maxHealth);
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
			heal(HEAL_AMOUNT);
			return;
		}
		else
		{
			powerup->type = type;
			powerup->level = 1;
			powerupType = type;
		}
		validatePowerup();
		powerupCooldown = true;
		lastPickupTime = (float)clock();
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
		// Switch physics shapes
		shapeToRemove = powerup->shape;
		powerup->shape = physicsManager->physics->createShape(PxBoxGeometry(0.2f * powerup->level, 0.3f, 0.3f), *material);
		powerup->shape->setLocalPose(PxTransform(PxVec3(0.0f, 0.3f, 1.0f)));
		data->type = WEAPON_SHAPE;
		data->parent = this;
		powerup->shape->userData = data;
		//Reset other data
		maxHealth = BASE_MAX_HEALTH;
		health = std::min(health, maxHealth);
		damageReduce = 0;
		break;
	case RANGED_UPGRADE: 
		powerup->damage = BASE_RANGE_DAMAGE * powerup->level;
		// Remove physics shape if necessary
		shapeToRemove = powerup->shape;
		powerup->shape = NULL;
		// Reset other data
		maxHealth = BASE_MAX_HEALTH;
		health = std::min(health, maxHealth);
		damageReduce = 0;
		break;
	case SHIELD_UPGRADE: 
		powerup->damage = BASE_SHIELD_FORCE * powerup->level;	// For the shield, this affects the bounce effect, not actual damage
		maxHealth += SHIELD_HEALTH_BONUS;
		health += SHIELD_HEALTH_BONUS;
		damageReduce = SHIELD_DAMAGE_REDUCTION;
		// Remove physics shape if necessary
		shapeToRemove = powerup->shape;
		powerup->shape = NULL;
		break;
	}

	addPowerupShape = true;
}

void Roomba::getControl()
{
	control->steer = controller->getLeftThumbX(controllerIndex) / -32768.0f;
	control->accel = controller->getRightTrigger(controllerIndex) / 255.0f > controller->getLeftTrigger(controllerIndex) / 255.0f ? controller->getRightTrigger(controllerIndex) / 255.0f : controller->getLeftTrigger(controllerIndex) / -255.0f;
	control->braking = controller->getBDown(controllerIndex) ? 1.0f : 0.0f;
	control->handbrake = controller->getADown(controllerIndex) ? 1.0f : 0.0f;
	control->shooting = controller->getXDown(controllerIndex);
}

Projectile* Roomba::createProjectile(obj* model)
{
	vec3 position = this->position;
	vec3 direction = vec3(2 * (rotation.x * rotation.z + rotation.w * rotation.y),
		2 * (rotation.y * rotation.x - rotation.w * rotation.x),
		1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y));

	direction = normalize(direction);
	if (powerup->level == 1) {
		position += vec3(0.0f, 1.5f, 0.0f);
		direction += vec3(0.0f, 0.1f, 0.0f);
	}
	else if (powerup->level == 2) {
		position += direction * 1.7f + vec3(0.0f, 0.5f, 0.0f);
		direction *= 2.5f;
	}
	else if (powerup->level == 3) {
		position += direction * 1.7f + vec3(0.0f, 0.5f, 0.0f);
		direction *= 3.5f;
	}

	return new Projectile(physicsManager, position, direction, getDamage(), model);
}

void Roomba::applyForce(PxVec3* force)
{
	this->force->x += force->x;
	this->force->y += force->y;
	this->force->z += force->z;
}