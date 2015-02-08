#include "WheelQueryResults.h"
#include "PhysX/vehicle/PxVehicleSDK.h"
#include "PhysX/foundation/PsFoundation.h"
#include "PhysX/foundation/PsUtilities.h"


//#define CHECK_MSG(exp, msg) (!!(exp) || (physx::shdfnd::getFoundation().error(physx::PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__, msg), 0) )


WheelQueryResults* WheelQueryResults::allocate(const PxU32 maxNumWheels)
{
	const PxU32 size = sizeof(WheelQueryResults) + sizeof(PxWheelQueryResult)*maxNumWheels;
	WheelQueryResults* resData = (WheelQueryResults*)PX_ALLOC(size, PX_DEBUG_EXP("WheelQueryResults"));
	resData->init();
	PxU8* ptr = (PxU8*)resData;
	ptr += sizeof(WheelQueryResults);
	resData->mWheelQueryResults = (PxWheelQueryResult*)ptr;
	ptr += sizeof(PxWheelQueryResult)*maxNumWheels;
	resData->mMaxNumWheels = maxNumWheels;
	for (PxU32 i = 0; i<maxNumWheels; i++)
	{
		new(&resData->mWheelQueryResults[i]) PxWheelQueryResult();
	}
	return resData;
}

void WheelQueryResults::free()
{
	PX_FREE(this);
}

PxWheelQueryResult* WheelQueryResults::addVehicle(const PxU32 numWheels)
{
	PX_ASSERT((mNumWheels + numWheels) <= mMaxNumWheels);
	PxWheelQueryResult* r = &mWheelQueryResults[mNumWheels];
	mNumWheels += numWheels;
	return r;
}