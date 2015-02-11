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
#include "PhysX\vehicle\PxVehicleUtil.h"
#include "SceneQuery.h"
#include "WheelQueryResults.h"
#include <ctime>
#include <algorithm>
#include <iostream>
using namespace physx;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static const float MIN_FPS = 1.0f / 60.0f;
static const unsigned int MAX_VEHICLES = 12;
static const unsigned int NUM_WHEELS = 4;

//Tire types.
enum
{
	TIRE_TYPE_WETS = 0,
	TIRE_TYPE_SLICKS,
	TIRE_TYPE_ICE,
	TIRE_TYPE_MUD,
	MAX_NUM_TIRE_TYPES
};

//Collision types and flags describing collision interactions of each collision type.
enum
{
	COLLISION_FLAG_GROUND = 1 << 0,
	COLLISION_FLAG_WHEEL = 1 << 1,
	COLLISION_FLAG_CHASSIS = 1 << 2,
	COLLISION_FLAG_OBSTACLE = 1 << 3,
	COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

	COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
};

//Drivable surface types.
enum
{
	SURFACE_TYPE_MUD = 0,
	SURFACE_TYPE_TARMAC,
	SURFACE_TYPE_SNOW,
	SURFACE_TYPE_GRASS,
	MAX_NUM_SURFACE_TYPES
};

struct TireFrictionMultipliers
{
	static float getValue(PxU32 surfaceType, PxU32 tireType)
	{
		//Tire model friction for each combination of drivable surface type and tire type.
		static PxF32 tireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
		{
			//WETS	SLICKS	ICE		MUD
			{ 0.95f, 0.95f, 0.95f, 0.95f },		//MUD
			{ 1.10f, 1.15f, 1.10f, 1.10f },		//TARMAC
			{ 0.70f, 0.70f, 0.70f, 0.70f },		//ICE
			{ 0.80f, 0.80f, 0.80f, 0.80f }		//GRASS
		};
		return tireFrictionMultipliers[surfaceType][tireType];
	}
};



class PhysicsManager
{
private:
	PxScene* scene;
	PxCooking* cooking;
	float timestep;
	float lastTime;
	float gravity;
	//Vehicle data
	PxVehicleWheels* vehicles[MAX_VEHICLES];
	PxVehicleWheelQueryResult vehicleWheelQueryResults[MAX_VEHICLES];
	//Number of vehicles currently allocated
	int numVehicles;
	//Friction from combinations of tire and surface types.
	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs;
	//sdk raycasts (for the suspension lines).
	SceneQueryData* sqData;
	PxBatchQuery* sqWheelRaycastBatchQuery;
	//Reports for each wheel.
	WheelQueryResults* wheelQueryResults;
	//Cached simulation data of focus vehicle in 4W mode.
	PxVehicleWheelsSimData* wheelsSimData;
	PxVehicleDriveSimData4W driveSimData;
	//Material data
	PxVehicleDrivableSurfaceType	vehicleDrivableSurfaceTypes[16];
	PxMaterial*	standardMaterials[16];
	PxMaterial* chassisMaterialDrivable;
	PxMaterial* chassisMaterialNonDrivable;
	
public:
	PxPhysics* physics;

	PhysicsManager();
	~PhysicsManager();

	void Update(float steer, float accel, float braking);
	void LateUpdate();

	PxRigidDynamic* addDynamicObject(PxShape* shape, PxVec3 location, float density);
	PxRigidStatic* addStaticObject(PxTriangleMesh* shape, PxVec3 location);

	void createStandardMaterials();
	void suspensionRaycasts();
	void computeWheelWidthsAndRadii(PxConvexMesh** wheelConvexMeshes, PxF32* wheelWidths, PxF32* wheelRadii);
	PxVec3 computeChassisAABBDimensions(const PxConvexMesh* chassisConvexMesh);
	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts);
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* faces, const PxU32 numFaces);
	void vehicleSimulationSetup(const PxF32 chassisMass, PxConvexMesh* chassisConvexMesh,
		const PxF32 wheelMass, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets,
		PxVehicleWheelsSimData& wheelsData, PxVehicleDriveSimData4W& driveData, PxVehicleChassisData& chassisData);
	PxRigidDynamic* createVehicleActor(const PxVehicleChassisData& chassisData, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets4, 
		PxConvexMesh* chassisConvexMesh, const PxMaterial& material);
	PxRigidDynamic* createVehicle(const PxMaterial& material,const PxF32 chassisMass, const PxVec3* wheelCentreOffsets4, 
		PxConvexMesh* chassisConvexMesh, PxConvexMesh** wheelConvexMeshes4, const PxTransform& startTransform);

	
};

