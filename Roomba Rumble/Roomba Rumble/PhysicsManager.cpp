#include "PhysicsManager.h"
#include "Roomba.h"

#define PHYSX_DEBUGGER 1

PhysicsManager* PhysicsManager::mainPhysicsManager = NULL;

PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND;
	}

	return PxFilterFlag::eDEFAULT;
}

/*
The constructor does the following:
1) Set up the physics context.
2) Create a scene.
3) Hook up to the Visual Debugger if enabled.
*/
PhysicsManager::PhysicsManager()
{
	if (mainPhysicsManager == NULL) mainPhysicsManager = this;

	gravity = -60.0f;
	numVehicles = 0;
	lastTime = (float)clock();
	
	// Set up some stuff.
	PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, gDefaultErrorCallback);
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

	sceneDesc.filterShader = FilterShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

	// Create the scene.
	scene = physics->createScene(sceneDesc);

	// Initialize vehicle physics.
	PxInitVehicleSDK(*physics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	for (PxU32 i = 0; i<MAX_VEHICLES; i++)
	{
		vehicles[i] = NULL;
	}

	//Allocate simulation data so we can switch from 3-wheeled to 4-wheeled cars by switching simulation data.
	wheelsSimData = PxVehicleWheelsSimData::allocate(4);

	//Scene query data for to allow raycasts for all suspensions of all vehicles.
	sqData = SceneQueryData::allocate(MAX_VEHICLES * NUM_WHEELS);

	//Data to store reports for each wheel.
	wheelQueryResults = WheelQueryResults::allocate(MAX_VEHICLES * NUM_WHEELS);

	//Setup standard materials
	createStandardMaterials();

	defaultActorData = new ActorData();

	//Set up the friction values arising from combinations of tire type and surface type.
	surfaceTirePairs = PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);
	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, (const PxMaterial**)standardMaterials, vehicleDrivableSurfaceTypes);
	for (PxU32 i = 0; i<MAX_NUM_SURFACE_TYPES; i++)
	{
		for (PxU32 j = 0; j<MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, TireFrictionMultipliers::getValue(i, j));
		}
	}

	// If constant set, hook up to the visual debugger.
#if PHYSX_DEBUGGER == 1
	if (physics->getPvdConnectionManager() != NULL) {
		PxVisualDebuggerConnectionFlags connFlags(PxVisualDebuggerExt::getAllConnectionFlags());
		PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 10, connFlags);
	}
#endif
}


PhysicsManager::~PhysicsManager()
{
	if (mainPhysicsManager == this) mainPhysicsManager = NULL;
	delete defaultActorData;
	physics->release();
	mFoundation->release();
}

//Smoothing data for keyboard input
PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		3.0f,	//rise rate eANALOG_INPUT_ACCEL		
		3.0f,	//rise rate eANALOG_INPUT_BRAKE		
		10.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT	
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT	
	},
	{
		5.0f,	//fall rate eANALOG_INPUT__ACCEL		
		5.0f,	//fall rate eANALOG_INPUT__BRAKE		
		10.0f,	//fall rate eANALOG_INPUT__HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT	
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT	
	}
};
//Smoothing data for gamepad input
PxVehiclePadSmoothingData gCarPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL		
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		12.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		10.0f,	//rise rate eANALOG_INPUT_STEER_LEFT	
		10.0f,	//rise rate eANALOG_INPUT_STEER_RIGHT	
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL		
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		12.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		10.0f,	//fall rate eANALOG_INPUT_STEER_LEFT	
		10.0f	//fall rate eANALOG_INPUT_STEER_RIGHT	
	}
};
PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 0.6f,
	5.0f, 0.8f,
	7.5f, 1.0f,
	10.0f, 1.0f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

/*
Scene simulation. Assumes a minimum FPS as defined in the header.
*/
void PhysicsManager::Update()
{
	float time = (float)clock();
	timestep = std::min(MIN_FPS, (time - lastTime) / CLOCKS_PER_SEC);
	if (timestep <= 0) timestep = 0.001f;	// Edge case for when the clock goes weird.
	lastTime = time;

	suspensionRaycasts();
	
	PxVehicleUpdates(timestep, scene->getGravity(), *surfaceTirePairs, numVehicles, vehicles, vehicleWheelQueryResults);
	scene->simulate(timestep);
}

/*
Make scene simulation available to all objects.
*/
void PhysicsManager::LateUpdate()
{
	while (scene->fetchResults() == false);
}

/*
Adds a dynamic object to the scene. 
*/
PxRigidDynamic* PhysicsManager::addDynamicObject(PxGeometry* shape, PxVec3 location, float density)
{
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_OBSTACLE;
	simFilterData.word1 = COLLISION_FLAG_OBSTACLE_AGAINST;

	PxRigidDynamic* object = physics->createRigidDynamic(PxTransform(location));
	PxShape* newShape = object->createShape(*shape, *standardMaterials[SURFACE_TYPE_TARMAC]);
	PxRigidBodyExt::updateMassAndInertia(*object, density);
	newShape->setSimulationFilterData(simFilterData);

	PxFilterData qryFilterData;
	SampleVehicleSetupDrivableShapeQueryFilterData(&qryFilterData);

	newShape->setSimulationFilterData(simFilterData);
	newShape->setQueryFilterData(qryFilterData);

	object->userData = defaultActorData;
	newShape->userData = defaultActorData;

	scene->addActor(*object);

	return object;
}

PxRigidDynamic* PhysicsManager::addTriggerObject(PxGeometry* shape, PxVec3 location, float density)
{
	PxRigidDynamic* actor = addDynamicObject(shape, location, density);
	PxShape* actorShape;
	actor->getShapes(&actorShape, 1);
	actorShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	actorShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	
	return actor;
}


PxShape* PhysicsManager::addShape(PxShape* shape, PxRigidDynamic* actor)
{
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_EXTRA_SHAPE;
	simFilterData.word1 = COLLISION_FLAG_EXTRA_SHAPE_AGAINST;
	shape->setSimulationFilterData(simFilterData);
	if(shape->userData == NULL) shape->userData = defaultActorData;
	actor->attachShape(*shape);

	return shape;
}

void PhysicsManager::removeShape(PxShape* shape, PxRigidDynamic* actor)
{
	if (shape->userData != defaultActorData) delete shape->userData;
	actor->detachShape(*shape);
}

void PhysicsManager::setParent(void* parent, PxRigidDynamic* actor)
{
	((ActorData*)actor->userData)->parent = parent;
	PxShape** buffer = (PxShape**)malloc(sizeof(PxShape) * actor->getNbShapes());
	actor->getShapes(buffer, sizeof(PxShape) * actor->getNbShapes());
	for (PxU32 i = 0; i < actor->getNbShapes(); i++)
	{
		((ActorData*)buffer[i]->userData)->parent = parent;
	}
	delete buffer;
}

PxRigidStatic* PhysicsManager::addStaticObject(PxTriangleMesh* shape, PxVec3 location)
{
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_GROUND;
	simFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
	PxFilterData qryFilterData;
	SampleVehicleSetupDrivableShapeQueryFilterData(&qryFilterData);

	PxTriangleMeshGeometry triGeom;
	triGeom.triangleMesh = shape;
	PxRigidStatic* object = physics->createRigidStatic(PxTransform(location));
	PxShape* newShape =  object->createShape(triGeom, *standardMaterials[SURFACE_TYPE_TARMAC]);
	newShape->setSimulationFilterData(simFilterData);
	newShape->setQueryFilterData(qryFilterData);

	newShape->userData = defaultActorData;
	object->userData = defaultActorData;

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


	return physics->createTriangleMesh(rb);
}

PxVec3 PhysicsManager::computeChassisAABBDimensions(const PxConvexMesh* chassisConvexMesh)
{
	const PxU32 numChassisVerts = chassisConvexMesh->getNbVertices();
	const PxVec3* chassisVerts = chassisConvexMesh->getVertices();
	PxVec3 chassisMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
	PxVec3 chassisMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
	for (PxU32 i = 0; i<numChassisVerts; i++)
	{
		chassisMin.x = PxMin(chassisMin.x, chassisVerts[i].x);
		chassisMin.y = PxMin(chassisMin.y, chassisVerts[i].y);
		chassisMin.z = PxMin(chassisMin.z, chassisVerts[i].z);
		chassisMax.x = PxMax(chassisMax.x, chassisVerts[i].x);
		chassisMax.y = PxMax(chassisMax.y, chassisVerts[i].y);
		chassisMax.z = PxMax(chassisMax.z, chassisVerts[i].z);
	}
	const PxVec3 chassisDims = chassisMax - chassisMin;
	return chassisDims;
}

void PhysicsManager::computeWheelWidthsAndRadii(PxConvexMesh** wheelConvexMeshes, PxF32* wheelWidths, PxF32* wheelRadii)
{
	for (PxU32 i = 0; i<4; i++)
	{
		const PxU32 numWheelVerts = wheelConvexMeshes[i]->getNbVertices();
		const PxVec3* wheelVerts = wheelConvexMeshes[i]->getVertices();
		PxVec3 wheelMin(PX_MAX_F32, PX_MAX_F32, PX_MAX_F32);
		PxVec3 wheelMax(-PX_MAX_F32, -PX_MAX_F32, -PX_MAX_F32);
		for (PxU32 j = 0; j<numWheelVerts; j++)
		{
			wheelMin.x = PxMin(wheelMin.x, wheelVerts[j].x);
			wheelMin.y = PxMin(wheelMin.y, wheelVerts[j].y);
			wheelMin.z = PxMin(wheelMin.z, wheelVerts[j].z);
			wheelMax.x = PxMax(wheelMax.x, wheelVerts[j].x);
			wheelMax.y = PxMax(wheelMax.y, wheelVerts[j].y);
			wheelMax.z = PxMax(wheelMax.z, wheelVerts[j].z);
		}
		wheelWidths[i] = wheelMax.x - wheelMin.x;
		wheelRadii[i] = PxMax(wheelMax.y, wheelMax.z)*0.975f;
	}
}

void PhysicsManager::vehicleSimulationSetup
(const PxF32 chassisMass, PxConvexMesh* chassisConvexMesh,
const PxF32 wheelMass, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets,
PxVehicleWheelsSimData& wheelsData, PxVehicleDriveSimData4W& driveData, PxVehicleChassisData& chassisData)
{
	//Extract the chassis AABB dimensions from the chassis convex mesh.
	const PxVec3 chassisDims = computeChassisAABBDimensions(chassisConvexMesh);

	//The origin is at the center of the chassis mesh.
	//Set the center of mass to be below this point
	const PxVec3 chassisCMOffset = PxVec3(0.0f, -chassisDims.y*0.5f, 0.0f);

	//Now compute the chassis mass and moment of inertia.
	//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
	PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);

	//Let's set up the chassis data structure now.
	chassisData.mMass = chassisMass;
	chassisData.mMOI = chassisMOI;
	chassisData.mCMOffset = chassisCMOffset;

	//Compute the sprung masses of each suspension spring using a helper function.
	PxF32 suspSprungMasses[4];
	PxVehicleComputeSprungMasses(4, wheelCentreOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses);

	//Extract the wheel radius and width from the wheel convex meshes.
	PxF32 wheelWidths[4];
	PxF32 wheelRadii[4];
	computeWheelWidthsAndRadii(wheelConvexMeshes, wheelWidths, wheelRadii);

	//Now compute the wheel masses and inertias components around the axle's axis.
	//http://en.wikipedia.org/wiki/List_of_moments_of_inertia
	PxF32 wheelMOIs[4];
	for (PxU32 i = 0; i<4; i++)
	{
		wheelMOIs[i] = 0.5f*wheelMass*wheelRadii[i] * wheelRadii[i];
	}
	//Let's set up the wheel data structures now with radius, mass, and moi.
	PxVehicleWheelData wheels[4];
	for (PxU32 i = 0; i<4; i++)
	{
		wheels[i].mRadius = wheelRadii[i];
		wheels[i].mMass = wheelMass;
		wheels[i].mMOI = wheelMOIs[i];
		wheels[i].mWidth = wheelWidths[i];
	}
	//Disable the handbrake from the rear wheels and enable for the front wheels
	wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 0.0f;
	wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 0.0f;
	wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxHandBrakeTorque = 100.0f;
	wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxHandBrakeTorque = 100.0f;
	//Enable steering for the rear wheels and disable for the front wheels.
	wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxSteer = PxPi*0.21f;
	wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxSteer = PxPi*0.21f;
	wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = 0.01f;
	wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = 0.01f;

	//Let's set up the tire data structures now.
	//Put slicks on the front tires and wets on the rear tires.
	PxVehicleTireData tires[4];
	tires[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mType = TIRE_TYPE_ICE;
	tires[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mType = TIRE_TYPE_ICE;
	tires[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mType = TIRE_TYPE_ICE;
	tires[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mType = TIRE_TYPE_ICE;

	//Let's set up the suspension data structures now.
	PxVehicleSuspensionData susps[4];
	for (PxU32 i = 0; i<4; i++)
	{
		susps[i].mMaxCompression = 0.0f;
		susps[i].mMaxDroop = 0.2f;
		susps[i].mSpringStrength = 6.0f;
		susps[i].mSpringDamperRate = 4.0f;
	}
	susps[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mSprungMass = suspSprungMasses[PxVehicleDrive4WWheelOrder::eFRONT_LEFT];
	susps[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mSprungMass = suspSprungMasses[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT];
	susps[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mSprungMass = suspSprungMasses[PxVehicleDrive4WWheelOrder::eREAR_LEFT];
	susps[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mSprungMass = suspSprungMasses[PxVehicleDrive4WWheelOrder::eREAR_RIGHT];

	//Set up the camber.
	//Remember that the left and right wheels need opposite camber so that the car preserves symmetry about the forward direction.
	const PxF32 camberAngleAtRest = 0.000f;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtRest = camberAngleAtRest;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtRest = -camberAngleAtRest;
	susps[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtRest = 0.0f;
	susps[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtRest = 0.0f;
	//Set the wheels to tilt less at maximum droop
	const PxF32 camberAngleAtMaxDroop = 0.020f;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtMaxDroop = camberAngleAtMaxDroop;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
	susps[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtMaxDroop = 0.0f;
	susps[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtMaxDroop = 0.0f;
	//Set the wheels to tilt more at maximum compression
	const PxF32 camberAngleAtMaxCompression = -0.010f;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mCamberAtMaxCompression = camberAngleAtMaxCompression;
	susps[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
	susps[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mCamberAtMaxCompression = 0.0f;
	susps[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mCamberAtMaxCompression = 0.0f;

	//We need to set up geometry data for the suspension, wheels, and tires.
	//We already know the wheel centers described as offsets from the actor center and the center of mass offset from actor center.
	//From here we can approximate application points for the tire and suspension forces.
	//Lets assume that the suspension travel directions are absolutely vertical.
	PxVec3 suspTravelDirections[4] = { PxVec3(0, -1, 0), PxVec3(0, -1, 0), PxVec3(0, -1, 0), PxVec3(0, -1, 0) };
	PxVec3 wheelCentreCMOffsets[4];
	PxVec3 suspForceAppCMOffsets[4];
	PxVec3 tireForceAppCMOffsets[4];
	for (PxU32 i = 0; i<4; i++)
	{
		wheelCentreCMOffsets[i] = wheelCentreOffsets[i] - chassisCMOffset;
		suspForceAppCMOffsets[i] = PxVec3(wheelCentreCMOffsets[i].x, -0.1f, wheelCentreCMOffsets[i].z);
		tireForceAppCMOffsets[i] = PxVec3(wheelCentreCMOffsets[i].x, -0.1f, wheelCentreCMOffsets[i].z);
	}

	//Now add the wheel, tire and suspension data.
	for (PxU32 i = 0; i<4; i++)
	{
		wheelsData.setWheelData(i, wheels[i]);
		wheelsData.setTireData(i, tires[i]);
		wheelsData.setSuspensionData(i, susps[i]);
		wheelsData.setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsData.setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsData.setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsData.setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
	}

	//Set the car to perform 3 sub-steps when it moves with a forwards speed of less than 5.0 
	//and with a single step when it moves at speed greater than or equal to 5.0.
	wheelsData.setSubStepCount(5.0f, 3, 1);


	//Now set up the differential, engine, gears, clutch, and ackermann steering.

	//Diff
	PxVehicleDifferential4WData diff;
	diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	driveData.setDiffData(diff);

	//Engine
	PxVehicleEngineData engine;
	engine.mPeakTorque = 3000.0f;
	engine.mMaxOmega = 1000.0f;
	driveData.setEngineData(engine);

	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime = 0.00001f;
	driveData.setGearsData(gears);

	//Clutch
	PxVehicleClutchData clutch;
	clutch.mStrength = 1000.0f;
	driveData.setClutchData(clutch);

	//Ackermann steer accuracy
	PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy = 1.0f;
	ackermann.mAxleSeparation = wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].z - wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].z;
	ackermann.mFrontWidth = wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].x - wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].x;
	ackermann.mRearWidth = wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].x - wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT].x;
	driveData.setAckermannGeometryData(ackermann);
}


PxRigidDynamic* PhysicsManager::createVehicleActor
(const PxVehicleChassisData& chassisData, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets4, PxConvexMesh* chassisConvexMesh, const PxMaterial& material)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = physics->createRigidDynamic(PxTransform(PxIdentity));

	//We need to add wheel collision shapes, their local poses, a material for the wheels, and a simulation filter for the wheels.
	PxConvexMeshGeometry frontLeftWheelGeom(wheelConvexMeshes[0]);
	PxConvexMeshGeometry frontRightWheelGeom(wheelConvexMeshes[1]);
	PxConvexMeshGeometry rearLeftWheelGeom(wheelConvexMeshes[2]);
	PxConvexMeshGeometry rearRightWheelGeom(wheelConvexMeshes[3]);
	const PxGeometry* wheelGeometries[4] = { &frontLeftWheelGeom, &frontRightWheelGeom, &rearLeftWheelGeom, &rearRightWheelGeom };
	const PxTransform wheelLocalPoses[4] = { PxTransform(wheelCentreOffsets4[0]), PxTransform(wheelCentreOffsets4[1]), PxTransform(wheelCentreOffsets4[2]), PxTransform(wheelCentreOffsets4[3]) };
	const PxMaterial& wheelMaterial = material;
	PxFilterData wheelCollFilterData;
	wheelCollFilterData.word0 = COLLISION_FLAG_WHEEL;
	wheelCollFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;

	//We need to add chassis collision shapes, their local poses, a material for the chassis, and a simulation filter for the chassis.
	PxConvexMeshGeometry chassisConvexGeom(chassisConvexMesh);
	const PxGeometry* chassisGeom = &chassisConvexGeom;
	const PxTransform chassisLocalPose = PxTransform(PxIdentity);
	const PxMaterial& chassisMaterial = material;
	PxFilterData chassisCollFilterData;
	chassisCollFilterData.word0 = COLLISION_FLAG_CHASSIS;
	chassisCollFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

	//Create a query filter data for the car to ensure that cars
	//do not attempt to drive on themselves.
	PxFilterData vehQryFilterData;
	vehQryFilterData.word3 = (PxU32)65535;

	//Add all the wheel shapes to the actor.
	for (PxU32 i = 0; i<4; i++)
	{
		PxShape* wheelShape = vehActor->createShape(*wheelGeometries[i], wheelMaterial);
		wheelShape->setQueryFilterData(vehQryFilterData);
		wheelShape->setSimulationFilterData(wheelCollFilterData);
		wheelShape->setLocalPose(wheelLocalPoses[i]);
		wheelShape->userData = defaultActorData;
	}

	//Add the chassis shape to the actor.
	PxShape* chassisShape = vehActor->createShape(*chassisGeom, chassisMaterial);
	chassisShape->setQueryFilterData(vehQryFilterData);
	chassisShape->setSimulationFilterData(chassisCollFilterData);
	chassisShape->setLocalPose(chassisLocalPose);

	chassisShape->userData = new ActorData();
	((ActorData*)chassisShape->userData)->type = CHASSIS_SHAPE;
	
	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

	vehActor->userData = new ActorData();
	((ActorData*)vehActor->userData)->type = ROOMBA_ACTOR;

	return vehActor;
}

/*
Creates a vehicle actor based on the given input.
The output is in two parts: the PxRigidDynamic is returned directly by the function and represents the physical representation of the vehicle. The index of the vehicle
in the vehicles array is also returned in vehActor->userData->parent for use during deletion as the variable will not be set yet.
*/
PxRigidDynamic* PhysicsManager::createVehicle(const PxMaterial& material, const PxF32 chassisMass, const PxVec3* wheelCentreOffsets4,
	PxConvexMesh* chassisConvexMesh, PxConvexMesh** wheelConvexMeshes4, const PxTransform& startTransform)
{
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(4);
	PxVehicleDriveSimData4W driveSimData;
	PxVehicleChassisData chassisData;
	vehicleSimulationSetup
		(chassisMass, chassisConvexMesh,
		WHEEL_MASS, wheelConvexMeshes4, wheelCentreOffsets4,
		*wheelsSimData, driveSimData, chassisData);

	//Instantiate and finalize the vehicle using physx.
	PxRigidDynamic* vehActor = createVehicleActor(chassisData, wheelConvexMeshes4, wheelCentreOffsets4, chassisConvexMesh, material);
	vehActor->setGlobalPose(PxTransform(startTransform));

	//Create a car.
	PxVehicleDrive4W* car = PxVehicleDrive4W::allocate(4);
	car->setup(physics, vehActor, *wheelsSimData, driveSimData, 0);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	scene->addActor(*vehActor);
	
	vehActor->setContactReportThreshold(2750.0f);

	//Set up the mapping between wheel and actor shape.
	car->mWheelsSimData.setWheelShapeMapping(0, 0);
	car->mWheelsSimData.setWheelShapeMapping(1, 1);
	car->mWheelsSimData.setWheelShapeMapping(2, 2);
	car->mWheelsSimData.setWheelShapeMapping(3, 3);

	//Set up the scene query filter data for each suspension line.
	PxFilterData vehQryFilterData;
	vehQryFilterData.word3 = (PxU32)65535;
	car->mWheelsSimData.setSceneQueryFilterData(0, vehQryFilterData);
	car->mWheelsSimData.setSceneQueryFilterData(1, vehQryFilterData);
	car->mWheelsSimData.setSceneQueryFilterData(2, vehQryFilterData);
	car->mWheelsSimData.setSceneQueryFilterData(3, vehQryFilterData);

	//Set the autogear mode of the instantiate car.
	car->mDriveDynData.setUseAutoGears(true);
	car->mDriveDynData.setToRestState();
	car->mDriveDynData.forceGearChange(PxVehicleGearsData::eTHIRD);	//Start in third gear since we accelerate better

	//Increment the number of vehicles
	vehicles[numVehicles] = car;
	vehicleWheelQueryResults[numVehicles].nbWheelQueryResults = 4;
	vehicleWheelQueryResults[numVehicles].wheelQueryResults = wheelQueryResults->addVehicle(4);

	//Switch to 3-wheel mode.
	PxVehicle4WEnable3WTadpoleMode(car->mWheelsSimData, car->mWheelsDynData, car->mDriveSimData);

	((ActorData*)vehActor->userData)->parent = (void*)numVehicles;
	numVehicles++;

	return vehActor;
}

void PhysicsManager::deleteVehicle(int index)
{
	vehicles[index] = NULL;
	vehicleWheelQueryResults[index].wheelQueryResults = NULL;
	for (int i = index + 1; i < numVehicles; i++)
	{
		vehicles[i - 1] = vehicles[i];
		vehicleWheelQueryResults[i - 1].wheelQueryResults = vehicleWheelQueryResults[i].wheelQueryResults;
		((Roomba*)((ActorData*)vehicles[i - 1]->getRigidDynamicActor()->userData)->parent)->decVehicleIndex();
	}
	vehicles[--numVehicles] = NULL;
	vehicleWheelQueryResults[numVehicles].wheelQueryResults = NULL;
}

void PhysicsManager::suspensionRaycasts()
{
	//Create a scene query if we haven't already done so.
	if (NULL == sqWheelRaycastBatchQuery)
	{
		sqWheelRaycastBatchQuery = sqData->setUpBatchedSceneQuery(scene);
	}
	//Raycasts.
	PxSceneReadLock scopedLock(*scene);
	PxVehicleSuspensionRaycasts(sqWheelRaycastBatchQuery, numVehicles, vehicles, sqData->getRaycastQueryResultBufferSize(), sqData->getRaycastQueryResultBuffer());
}

void PhysicsManager::inputControls(int vehIndex, DriveControl* control)
{
	PxVehicleDrive4WRawInputData rawInputData;
	PxVehicleDrive4W* test = (PxVehicleDrive4W*)vehicles[vehIndex];

	float accelVal =0.0f;

	if (control->accel > 0) {
		test->mDriveDynData.forceGearChange(PxVehicleGearsData::eTHIRD);
		control->reversing = false;
		accelVal = control->accel;
	}
	else if (control->accel < 0) {
		test->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		control->reversing = true;
		accelVal = control->accel *-1;
	}

	if(accelVal < 0.03f)
	{
		vehicles[vehIndex]->getRigidDynamicActor()->setAngularVelocity(PxVec3(0, 5.0f * control->steer, 0));
	}

	rawInputData.setAnalogAccel(accelVal);
	rawInputData.setAnalogBrake(control->braking);
	rawInputData.setAnalogHandbrake(control->handbrake);
	rawInputData.setAnalogSteer(control->steer);
	rawInputData.setGearUp(false);
	rawInputData.setGearDown(false);

	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gCarPadSmoothingData, gSteerVsForwardSpeedTable, rawInputData, timestep, PxVehicleIsInAir(vehicleWheelQueryResults[vehIndex]), *test);
}

void PhysicsManager::createStandardMaterials()
{
	const PxF32 restitutions[MAX_NUM_SURFACE_TYPES] = { 0.8f, 0.0f, 0.8f, 0.8f };
	const PxF32 staticFrictions[MAX_NUM_SURFACE_TYPES] = { 0.5f, 0.5f, 0.5f, 0.5f };
	const PxF32 dynamicFrictions[MAX_NUM_SURFACE_TYPES] = { 0.5f, 0.3f, 0.5f, 0.5f };

	for (PxU32 i = 0; i<MAX_NUM_SURFACE_TYPES; i++)
	{
		//Create a new material.
		standardMaterials[i] = physics->createMaterial(staticFrictions[i], dynamicFrictions[i], restitutions[i]);
		//Set up the drivable surface type that will be used for the new material.
		vehicleDrivableSurfaceTypes[i].mType = i;
	}
	chassisMaterialDrivable = physics->createMaterial(0.0f, 0.0f, 0.8f);
	chassisMaterialNonDrivable = physics->createMaterial(1.0f, 1.0f, 0.6f);
}

// Callback for collision detection
void PhysicsManager::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND)
		{
			if (((ActorData*)pairHeader.actors[0]->userData)->type == ROOMBA_ACTOR && ((ActorData*)pairHeader.actors[1]->userData)->type == ROOMBA_ACTOR)
			{
				if (((ActorData*)pairs[i].shapes[0]->userData)->type == WEAPON_SHAPE && ((ActorData*)pairs[i].shapes[1]->userData)->type == CHASSIS_SHAPE)
				{
					Roomba* victim = ((Roomba*)((ActorData*)pairs[i].shapes[1]->userData)->parent);
					Roomba* jerk = ((Roomba*)((ActorData*)pairs[i].shapes[0]->userData)->parent);

					int damage = ((Roomba*)((ActorData*)pairs[i].shapes[0]->userData)->parent)->getDamage();
					((Roomba*)((ActorData*)pairs[i].shapes[1]->userData)->parent)->doDamage(damage);
					sound->playSound("hurt.wav", victim->getPosition());

					if(((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->isDestroyed() < 0)
						((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->incKills();

					int hit = ((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->eIndex;
					((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->setLastHitBy(hit);
				}
				else if (((ActorData*)pairs[i].shapes[1]->userData)->type == WEAPON_SHAPE && ((ActorData*)pairs[i].shapes[0]->userData)->type == CHASSIS_SHAPE)
				{
					int damage = ((Roomba*)((ActorData*)pairs[i].shapes[1]->userData)->parent)->getDamage();
					((Roomba*)((ActorData*)pairs[i].shapes[0]->userData)->parent)->doDamage(damage);
					Roomba* jerk = ((Roomba*)((ActorData*)pairs[i].shapes[0]->userData)->parent);
					sound->playSound("hurt.wav", jerk->getPosition());

					if(((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->isDestroyed() < 0)
						((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->incKills();

					int hit = ((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->eIndex;
					((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->setLastHitBy(hit);
				}
				else if (((ActorData*)pairs[i].shapes[0]->userData)->type == CHASSIS_SHAPE && ((ActorData*)pairs[i].shapes[1]->userData)->type == CHASSIS_SHAPE)
				{
					((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->doDamage(BASE_CHASSIS_DAMAGE);
					((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->doDamage(BASE_CHASSIS_DAMAGE);
					Roomba* victim = ((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent);
					sound->playSound("bump.wav", victim->getPosition());

					if(((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->isDestroyed() < 0)
						((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->incKills();
					else if(((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->isDestroyed() < 0)
						((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->incKills();

					int hit = ((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->eIndex;
					((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->setLastHitBy(hit);

					hit = ((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->eIndex;
					((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->setLastHitBy(hit);
				}
			}
		}
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if (((ActorData*)pairHeader.actors[0]->userData)->type == ROOMBA_ACTOR && ((ActorData*)pairHeader.actors[1]->userData)->type == ROOMBA_ACTOR)
			{
				if (((ActorData*)pairs[i].shapes[0]->userData)->type == CHASSIS_SHAPE || ((ActorData*)pairs[i].shapes[1]->userData)->type == CHASSIS_SHAPE)
				{
					Roomba* victim = ((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent);
					Roomba* jerk = ((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent);

					if (victim->getPowerupType() == SHIELD_UPGRADE) {
						PxVec3 p1 = PxVec3(victim->getPosition().x, 0.0f, victim->getPosition().z);
						PxVec3 p2 = PxVec3(jerk->getPosition().x, 0.0f, jerk->getPosition().z);
						PxVec3 force = (p2 - p1) * (float)victim->getDamage();
						jerk->applyForce(&force);
					}
					if (jerk->getPowerupType() == SHIELD_UPGRADE) {
						PxVec3 p1 = PxVec3(victim->getPosition().x, 0.0f, victim->getPosition().z);
						PxVec3 p2 = PxVec3(jerk->getPosition().x, 0.0f, jerk->getPosition().z);
						PxVec3 force = (p1 - p2) * (float)jerk->getDamage();
						victim->applyForce(&force);
					}
				}
			}
			else if (((ActorData*)pairHeader.actors[0]->userData)->type == PROJECTILE_ACTOR)
			{
				if (((ActorData*)pairHeader.actors[1]->userData)->type == ROOMBA_ACTOR)
				{
					int damage = ((Projectile*)((ActorData*)pairHeader.actors[0]->userData)->parent)->getDamage();
					((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->doDamage(damage);
					Roomba* victim = ((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent);
					sound->playSound("hurt.wav", victim->getPosition());

					if(((Roomba*)((ActorData*)pairHeader.actors[1]->userData)->parent)->isDestroyed() < 0)
						((Projectile*)((ActorData*)pairHeader.actors[0]->userData)->parent)->originRoomba->incKills();
				}
				((Projectile*)((ActorData*)pairHeader.actors[0]->userData)->parent)->destroyFlag();
			}
			else if (((ActorData*)pairHeader.actors[1]->userData)->type == PROJECTILE_ACTOR)
			{
				if (((ActorData*)pairHeader.actors[0]->userData)->type == ROOMBA_ACTOR)
				{
					int damage = ((Projectile*)((ActorData*)pairHeader.actors[1]->userData)->parent)->getDamage();
					((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->doDamage(damage);
					Roomba* jerk = ((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent);
					sound->playSound("hurt.wav", jerk->getPosition());

					if(((Roomba*)((ActorData*)pairHeader.actors[0]->userData)->parent)->isDestroyed() < 0)
						((Projectile*)((ActorData*)pairHeader.actors[1]->userData)->parent)->originRoomba->incKills();
				}
				((Projectile*)((ActorData*)pairHeader.actors[1]->userData)->parent)->destroyFlag();
			}
		}
	}
}

void PhysicsManager::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		if (((ActorData*)pairs[i].triggerActor->userData)->type == POWERUP_ACTOR && ((ActorData*)pairs[i].otherShape->userData)->type == CHASSIS_SHAPE)
		{
			if (((Powerup*)((ActorData*)pairs[i].triggerActor->userData)->parent)->isDestroyed() < 0) continue;
			int type = ((Powerup*)((ActorData*)pairs[i].triggerActor->userData)->parent)->getType();
			//cout << type << endl;
			((Roomba*)((ActorData*)pairs[i].otherActor->userData)->parent)->addPowerup(type);
			Powerup* powerup = (Powerup*) ((ActorData*)pairs[i].triggerActor->userData)->parent;
			powerup->destroyFlag();
			sound->playSound("powerup.wav", powerup->getPosition());
		}
	}
}