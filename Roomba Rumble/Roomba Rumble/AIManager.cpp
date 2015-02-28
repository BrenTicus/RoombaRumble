#include "AIManager.h"


AIManager::~AIManager(void)
{
}

void AIManager::Update(){
	for (int i =0; i < allAIRoombas->size() ; i++){
		resultAIControl[i]->accel = 1.0f;
		resultAIControl[i]->steer = 1.0f;
		resultAIControl[i]->braking = 0.0f;

	}
}


vector<DriveControl*>* AIManager::getAIControls(){

	return &resultAIControl;

}

