#pragma once
#include "Entity.h"

class Roomba : public Entity
{
protected:
	PxVehicleDrive4W* car;
	obj* wheel;
	int health;
	int maxHealth;
public:
	ActorData* actor;
	Roomba(PhysicsManager* physicsManager_, vec3 position_);
	~Roomba();

	
	virtual void Update();

	int getHealth() { return health; }
	void setHeatlh(int h) { health = h; }
	int doDamage(int d) { health -= d; if (health < 0) cout << "DEAD" << endl; return health; }
	int heal(int h) { health += h; health = std::max(health, maxHealth); return health; }
};