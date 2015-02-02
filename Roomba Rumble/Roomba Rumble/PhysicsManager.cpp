#include "PhysicsManager.h"

#define PHYSX_DEBUGGER 1

/*
The constructor does the following:
1) Set up the physics context.
2) Create a scene.
3) Hook up to the Visual Debugger if enabled.
*/
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
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);

	// Set up the scene.
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, gravity, 0.0f);

	if (!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

	// Create the scene.
	scene = physics->createScene(sceneDesc);

	// Initialize vehicle physics.
	PxInitVehicleSDK(*physics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	// If constant set, hook up to the visual debugger.
#if PHYSX_DEBUGGER == 1
	if (physics->getPvdConnectionManager() != NULL) {
		PxVisualDebuggerConnectionFlags connFlags(PxVisualDebuggerExt::getAllConnectionFlags());
		PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 10, connFlags);
	}
#endif
	/*
	// Stick a plane there for testing purposes.
	PxMaterial* mMaterial = physics->createMaterial(0.10f, 0.05f, 0.10f);
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(PxVec3(0, 1, 0), 0), *mMaterial);
	scene->addActor(*plane);
	*/
}


PhysicsManager::~PhysicsManager()
{
}

/*
Scene simulation. Assumes a minimum FPS as defined in the header.
*/
void PhysicsManager::Update()
{
	scene->simulate(std::min((float)(time = clock() - time), MIN_FPS));
}

/*
Make scene simulation available to all objects.
*/
void PhysicsManager::LateUpdate()
{
	scene->fetchResults();
}

/*
Adds a dynamic object to the scene. 
*/
PxRigidDynamic* PhysicsManager::addDynamicObject(PxShape* shape, PxVec3 location, float density)
{
	PxRigidDynamic* object = physics->createRigidDynamic(PxTransform(location));
	object->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*object, density);

	scene->addActor(*object);

	return object;
}

PxRigidStatic* PhysicsManager::addStaticObject(PxTriangleMesh* shape, PxMaterial* material, PxVec3 location)
{
	PxTriangleMeshGeometry triGeom;
	triGeom.triangleMesh = shape;
	PxRigidStatic* object = physics->createRigidStatic(PxTransform(location));
	object->createShape(triGeom, *material);

	scene->addActor(*object);

	return object;
}

PxConvexMesh* PhysicsManager::createConvexMesh(const PxVec3* verts, const PxU32 numVerts)
{
	// Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if (cooking->cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics->createConvexMesh(id);
	}
	_ASSERT(convexMesh > 0);
	return convexMesh;
}

PxTriangleMesh* PhysicsManager::createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* faces, const PxU32 numFaces)
{
	PxTriangleMeshDesc triangleDesc;
	triangleDesc.points.count = numVerts;
	triangleDesc.points.stride = sizeof(PxVec3);
	triangleDesc.points.data = verts;
	triangleDesc.triangles.count = numFaces;
	triangleDesc.triangles.stride = sizeof(PxReal) * 3;
	triangleDesc.triangles.data = faces;
	triangleDesc.flags = PxMeshFlags(0);

	bool res = cooking->validateTriangleMesh(triangleDesc);
	
	PxDefaultMemoryOutputStream stream;
	bool ok = cooking->cookTriangleMesh(triangleDesc, stream);
	PxDefaultMemoryInputData rb(stream.getData(), stream.getSize());
	_ASSERT(ok == true);

	return physics->createTriangleMesh(rb);
}
