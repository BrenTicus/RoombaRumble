#include "PhysicsManager.h"


PhysicsManager::PhysicsManager()
{
	gravity = -9.81f;
	time = clock();
	
	// Set up some stuff.
	PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;
	PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, gDefaultErrorCallback);
	PxProfileZoneManager* mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);

	PxTolerancesScale scale;

	// Create the physics context.
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, true, mProfileZoneManager);

	// Set up slightly more understandable things.
	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES | PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES | PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	PxCooking* mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);

	// Set up the scene.
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, gravity, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

	// Create the scene.
	scene = physics->createScene(sceneDesc);

	// Stick a plane there for testing purposes.
	PxMaterial* mMaterial = physics->createMaterial(0.10f, 0.05f, 0.10f);
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(PxVec3(0, 1, 0), 0), *mMaterial);
	scene->addActor(*plane);
}


PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Update()
{
	scene->simulate(std::min((float)(time = clock() - time), 1.0f / 60.0f));
}

void PhysicsManager::LateUpdate()
{
	scene->fetchResults();
}

PxRigidDynamic* PhysicsManager::addDynamicObject(PxShape* shape, PxVec3 location, float density)
{
	PxRigidDynamic* object = physics->createRigidDynamic(PxTransform(location));
	object->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*object, density);

	scene->addActor(*object);

	return object;
}