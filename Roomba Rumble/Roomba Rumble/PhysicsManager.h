#pragma once

// I'm fairly certain these lines are unnecessary, so if anyone figures out how to get rid of them that would be cool.
#pragma comment(lib,"lib/PhysX3DEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib,"lib/PhysXProfileSDKDEBUG.lib")
#pragma comment(lib,"lib/PxTaskDEBUG.lib")
#pragma comment(lib,"lib/PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib,"lib/PhysX3VehicleDEBUG.lib")
#pragma comment(lib,"lib/PhysXVisualDebuggerSDKDEBUG.lib")

#include "PhysX\PxPhysicsAPI.h"
#include <ctime>
#include <algorithm>
#include <iostream>

using namespace physx;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static const float MIN_FPS = 1.0f / 60.0f;

class PhysicsManager
{
private:
	PxScene* scene;
	PxCooking* cooking;
	clock_t time;
	float gravity;
public:
	PxPhysics* physics;

	PhysicsManager();
	~PhysicsManager();

	void Update();
	void LateUpdate();

	PxRigidDynamic* addDynamicObject(PxShape* shape, PxVec3 location, float density);
	PxRigidStatic* addStaticObject(PxTriangleMesh* shape, PxMaterial* material, PxVec3 location);

	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts);
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* faces, const PxU32 numFaces);
};

