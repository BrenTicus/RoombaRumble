#pragma once
#include "PhysX/vehicle/PxVehicleSDK.h"
#include "PhysX/vehicle/PxVehicleUpdate.h"

using namespace physx;

//Data structure to store reports for each wheel. 
class WheelQueryResults
{
public:

	//Allocate a buffer of wheel query results for up to maxNumWheels.
	static WheelQueryResults* allocate(const PxU32 maxNumWheels);

	//Free allocated buffer.
	void free();

	PxWheelQueryResult* addVehicle(const PxU32 numWheels);

private:

	//One result for each wheel.
	PxWheelQueryResult* mWheelQueryResults;

	//Maximum number of wheels.
	PxU32 mMaxNumWheels;

	//Number of wheels 
	PxU32 mNumWheels;


	WheelQueryResults()
		: mWheelQueryResults(NULL), mMaxNumWheels(0), mNumWheels(0)
	{
		init();
	}

	~WheelQueryResults()
	{
	}

	void init()
	{
		mWheelQueryResults = NULL;
		mMaxNumWheels = 0;
		mNumWheels = 0;
	}
};