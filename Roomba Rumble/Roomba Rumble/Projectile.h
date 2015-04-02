#pragma once
#include "Entity.h"
#include "Roomba.h"
class Projectile :
	public Entity
{
protected:
	int damage;
public:
	class Roomba* originRoomba;

	Projectile(PhysicsManager* physicsManager, vec3 position, vec3 direction, PxQuat rotation, int damage, Roomba* roomba);
	~Projectile();
	int getDamage() { return damage; }
};

