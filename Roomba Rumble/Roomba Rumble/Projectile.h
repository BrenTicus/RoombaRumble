#pragma once
#include "Entity.h"
class Projectile :
	public Entity
{
protected:
	int damage;
public:
	Projectile(PhysicsManager* physicsManager, vec3 position, vec3 direction, int damage);
	~Projectile();
	int getDamage() { return damage; }
};

