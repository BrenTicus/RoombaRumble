#include "Roomba.h"
#include "EntityManager.h"
#include "Sound.h"

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
	
	PxVec3 wheelOffsets[4] = { PxVec3(-0.5f, 0.0f, 0.7f), PxVec3(0.5f, 0.0f, 0.7f), PxVec3(-0.5f, 0.0f, -0.6f), PxVec3(0.5f, 0.0f, -0.6f) };
	PxConvexMesh* wheelMeshes[4] = { wheelMesh, wheelMesh, wheelMesh, wheelMesh };

	hitbox = physicsManager->createVehicle(*material, CHASSIS_MASS, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	vehicleIndex = (int)((ActorData*)hitbox->userData)->parent;
	physicsManager->setParent(this, hitbox);

	// Initialize the weapon.
	powerup = new weapon();
	powerup->damage = BASE_CHASSIS_DAMAGE;
	powerup->level = 0;
	powerup->type = NO_UPGRADE;
	kills = 0;

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
	invincibleTimer = 0.0f;
	invincibleMode = false;
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

	hitbox = physicsManager->createVehicle(*material, CHASSIS_MASS, wheelOffsets, mesh, wheelMeshes, PxTransform(PxVec3(position.x, position.y, position.z)));
	vehicleIndex = (int)((ActorData*)hitbox->userData)->parent;
	physicsManager->setParent(this, hitbox);

	// Initialize the weapon.
	powerup = new weapon();
	powerup->damage = 1;
	powerup->level = 0;
	//powerup->model = new obj();
	powerup->type = NO_UPGRADE;
	kills = 0;

	powerupAttached = false;
}

Roomba::~Roomba()
{
	delete control;
	delete powerup;
	delete force;
}

void Roomba::Destroy()
{
	physicsManager->deleteVehicle(vehicleIndex);
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
	force->x = 0; force->y = 0; force->z = 0;
	if (powerup->type == RANGED_UPGRADE)
	{
		if (!destroy && control->shooting && clock() - lastShotTime > MAX_SHOT_COOLDOWN)
		{
			lastShotTime = (float)clock();
			Projectile* proj = createProjectile();
			vec3 pos = proj->getPosition();
			proj->setTag("projectile");
			EntityManager::mainEntityManager->entityList.push_back(proj);
			Sound::mainSound->playSound("elastic.aiff", pos); //http://www.freesound.org/people/beskhu/sounds/149602/ 
		}
	}

	
	UpdateInvincible();

	return Entity::Update();
}

void Roomba::UpdateInvincible()
{
	//printf("#%d", vehicleIndex);
	//printf(invincibleMode ? "true\n" : "false\n");

	//make roomba invincible for a bit
	if (invincibleMode == false){
		//mode not on, do not allow
		invincibleTimer = (float)clock();
		invincibleFlashTimer = (float)clock();
	}

	if(clock() - invincibleTimer > invincibleCooldown){
		invincibleMode = false;
		invincibleTimer = (float)clock();
	}
	
	//flashing effect when invincible
	if(clock() - invincibleFlashTimer > INVINCIBLE_FLASH_COOLDOWN){
		invincibleFlashTimer = (float)clock();
		//printf("derk\n");

	}
}


void Roomba::activate(glm::vec3 position)
{
	hitbox->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	this->position = position;
	hitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z)), true);
	aliveFlag();
	invincibleMode = true;
	invincibleCooldown = INVINCIBLE_RESPAWN_COOLDOWN;
	activated = true;
}

void Roomba::deactivate()
{
	this->position.y += 1000.0f;
	hitbox->setGlobalPose(PxTransform(PxVec3(position.x, position.y, position.z)), true);
	hitbox->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	
	maxHealth = BASE_MAX_HEALTH;
	health = maxHealth;
	powerup->damage = BASE_CHASSIS_DAMAGE;
	powerup->level = 0;
	powerup->type = NO_UPGRADE;
	if (powerup->shape)  {
		addPowerupShape = true;
		shapeToRemove = powerup->shape;
	}

	powerupAttached = false;
	activated = false;
	aliveFlag();
}

int Roomba::doDamage(int d)
{

	if ( invincibleMode == false){
		health = health - (d - damageReduce); 
		invincibleMode = true;
		invincibleCooldown = INVINCIBLE_HIT_COOLDOWN;
	}

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
		}
		validatePowerup();
		powerupCooldown = true;
		lastPickupTime = (float)clock();
	}
}

void Roomba::validatePowerup()
{
	if (powerup->level > 3) { powerup->level = 3; return; }
	switch (powerup->type)
	{
	case MELEE_UPRADE:
	{
		powerup->damage = BASE_MELEE_DAMAGE * powerup->level;
		// Switch physics shapes
		shapeToRemove = powerup->shape;
		powerup->shape = physicsManager->physics->createShape(PxBoxGeometry(0.2f * powerup->level, 0.3f, 0.3f), *material);
		powerup->shape->setLocalPose(PxTransform(PxVec3(0.0f, 0.3f, 1.0f)));
		ActorData* data = new ActorData();
		data->type = WEAPON_SHAPE;
		data->parent = this;
		powerup->shape->userData = data;
		//Reset other data
		maxHealth = BASE_MAX_HEALTH;
		health = std::min(health, maxHealth);
		damageReduce = 0;
	}
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
	if (controller->getYDown(controllerIndex) && (clock() - lastJumpTime) / CLOCKS_PER_SEC > JUMP_COOLDOWN)
	{
		lastJumpTime = (float)clock();
		PxVec3 jump = PxVec3(2 * (rotation.x * rotation.z + rotation.w * rotation.y),
			2 * (rotation.y * rotation.x - rotation.w * rotation.x),
			1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y));
		jump.normalize();
		jump = 5 * jump;
		jump = jump + PxVec3(0.0f, 130.0f, 0.0f);
		applyForce(&jump);
	}
}

Projectile* Roomba::createProjectile()
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
		vec3 target = EntityManager::mainEntityManager->nearestRoomba(position);
		position += vec3(0.0f, 1.5f, 0.0f);
		direction = normalize(target - position);
		direction *= 1.5f;
	}

	return new Projectile(physicsManager, position, direction, hitbox->getGlobalPose().q, getDamage(), this);
}

void Roomba::applyForce(PxVec3* force)
{
	this->force->x += force->x;
	this->force->y += force->y;
	this->force->z += force->z;
}