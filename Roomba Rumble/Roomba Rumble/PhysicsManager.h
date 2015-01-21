#pragma once

#include "PhysX\PxPhysicsAPI.h"

#pragma comment(lib,"lib/PhysX3DEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysXProfileSDKDEBUG.lib")
#pragma comment(lib,"lib/PxTaskDEBUG.lib")
#pragma comment(lib,"lib/PhysX3ExtensionsDEBUG.lib")

using namespace physx;

static PxPhysics* mPhysics = NULL;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;

class PhysicsManager
{
private:
	PxPhysics* physics;
	PxScene* scene;
public:
	PhysicsManager();
	~PhysicsManager();

	int Update();
};

