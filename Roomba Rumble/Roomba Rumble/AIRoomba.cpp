#include "AIRoomba.h"
#include <iostream>

static int globalID;


AIRoomba::~AIRoomba()
{

	//update global ID so that its unique for each AI roomba
	globalID = globalID % 1000;
	id = globalID;
	globalID++;



	//intailize instance variables
	cycle = 0;
	action = "";

	control->accel = 0;
	control->steer = 0;
	control->braking = 0;
	control->reversing = 0;

}

void AIRoomba::Destroy()
{
	alive = false;
	Roomba::Destroy();
}






//given two position vectors, get the distance
static float getDistance(vec3 pos1, vec3 pos2){
	//printf("POS1 (%f, %f, %f)\n", pos1.x, pos1.y, pos1.z);
	//printf("POS2 (%f, %f, %f)\n", pos2.x, pos2.y, pos2.z);
	vec3 diff = pos1 - pos2;
	return glm::sqrt(diff.x*diff.x + diff.y* diff.y + diff.z* diff.z);
}


const float ACCEL_DISTANCE_CAP = 20.0f;

//returns acceleration between 0.0 - 1.0 as one approaches
static float accelApproach(float distance){
	float min = distance >= ACCEL_DISTANCE_CAP ? ACCEL_DISTANCE_CAP : distance;
	return min / ACCEL_DISTANCE_CAP;
}


static vec3 normalize(vec3 v){
	float length = glm::sqrt(v.x*v.x + v.y* v.y + v.z* v.z);
	vec3 newV;
	newV.x = v.x / length;
	newV.y = v.y /length;
	newV.z = v.z /length;
	return newV;
}

static float length(vec3 v){ return glm::sqrt(v.x*v.x + v.y* v.y + v.z* v.z);}


const float STEER_BUFFER_DELTA = 0.25f;

vec3 getForwardVector(quat q) 
{
    return vec3( 2 * (q.x * q.z + q.w * q.y), 
                    2 * (q.y * q.x - q.w * q.x),
                    1 - 2 * (q.x * q.x + q.y * q.y));
}

//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer,Entity* who, Entity* to){

	vec3 whoDir(0,0,0);


	whoDir = getForwardVector(who->getRotation());
	vec3 toDir =  (to->getPosition() - who->getPosition());


	toDir = glm::normalize(toDir);
	whoDir = glm::normalize(whoDir);

	vec3 diff = (toDir - whoDir);


	//printf("whoDir Z %f Y %f X %f\n", whoDir.z, whoDir.y, whoDir.x);
	//printf("toDir Z %f Y %f X %f\n", toDir.z, toDir.y, toDir.x);
	//printf("diffDir Z %f Y %f X %f\n", diff.z, diff.y, diff.x);
	float negation = (toDir.x) > 0.0f ? 1.0f : -1.0f;

	
	//float dot = whoDir.x * toDir.x + whoDir.y * toDir.y + whoDir.z * toDir.z;
	float dot = length(diff);

	if ((diff.z >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.z <= (1.0 * STEER_BUFFER_DELTA)) && (diff.x >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.x <= (1.0 * STEER_BUFFER_DELTA))){
		//printf("DRIVE STRAIGHT");
		buffer->steer = 0.0f;
		//buffer->steer = 0.75f * negation;
	}
	else{
		buffer->steer = 0.75f * negation;
	}
	
	//buffer->steer = negation;
	buffer->accel = 0.35f;//accelApproach(getDistance(who->getPosition(), to->getPosition()));
	buffer->braking = 0.0;


}

void driveTo90(DriveControl* buffer, Entity* who, bool turnRight){

}



const float AWARE_DISTANCE = 390.0f;


const int UPDATE_CHECK = 1000;
const int STUCK_CHECK = 1000;

int AIRoomba::UpdateAI(std::vector<Entity*>* entityList)
{


	for (unsigned int j = 0; j < entityList->size(); j++){

		Entity* curAI = entityList->at(j);

			if ((strcmp(entityList->at(j)->getTag(), "roomba") == 0)){// || (strcmp(entityList[j]->getTag(), "airoomba") == 0)){
				//if not self
				float entityDistance = getDistance(curAI->getPosition(), entityList->at(j)->getPosition());

				if (entityDistance <= AWARE_DISTANCE){
					//within field of chase

					//printf("dist %f\n", entityDistance);
					driveTowards(control, this, entityList->at(j));
					//printf("Will steer %s\n", aiControls[i]->steer >=0 ? "RIGHT" : "LEFT");
				}
			}

		}




	if (cycle >= UPDATE_CHECK){


		if (strcmp(action, "get_weapon") == 0){
			//fetch weapon

		}
		else if (strcmp(action, "attack") == 0){
			//ATTACK!

		}
		else if (strcmp(action, "escape_stuck") == 0){
			//escape the stuckness

		}
		else if (strcmp(action, "camp") == 0){
			//wait cause we boring

		}
		else{
			//nothing, change to new action

		}


		cycle = 0;
	}


	cycle++;

	return 0;
}



int AIRoomba::Update(){
	return Roomba::Update();
}