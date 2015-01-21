#include "PhysicsManager.h"


PhysicsManager::PhysicsManager()
{
	PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;
	PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, gDefaultErrorCallback);
	PxProfileZoneManager* mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);

	PxTolerancesScale scale;

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, true, mProfileZoneManager);

	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	PxCooking* mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -20.0f, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

	scene = physics->createScene(sceneDesc);
}


PhysicsManager::~PhysicsManager()
{
}

int PhysicsManager::Update()
{
	return 0;
}