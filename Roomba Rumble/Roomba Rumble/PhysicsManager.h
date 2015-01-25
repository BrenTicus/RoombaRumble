#pragma once

// I'm fairly certain these lines are unnecessary, so if anyone figures out how to get rid of them that would be cool.
#pragma comment(lib,"lib/PhysX3DEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysXProfileSDKDEBUG.lib")
#pragma comment(lib,"lib/PxTaskDEBUG.lib")
#pragma comment(lib,"lib/PhysX3ExtensionsDEBUG.lib")

#include "PhysX\PxPhysicsAPI.h"
#include <ctime>
#include <algorithm>

using namespace physx;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;

class PhysicsManager
{
private:
	PxScene* scene;
	clock_t time;
	float gravity;
public:
	PxPhysics* physics;

	PhysicsManager();
	~PhysicsManager();

	void Update();
	void LateUpdate();

	PxRigidDynamic* addDynamicObject(PxShape* shape, PxVec3 location, float density);
};

