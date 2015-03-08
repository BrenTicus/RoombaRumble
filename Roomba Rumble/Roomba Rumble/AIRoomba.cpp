#include "AIRoomba.h"
#include <iostream>





AIRoomba::~AIRoomba()
{

	
}

void AIRoomba::Destroy()
{
	alive = false;
	Roomba::Destroy();
}


//returns a random number between min and max inclusive
int AIRoomba::getRandInt(int min, int max){
	return (randGen() % (max - min + 1)) + min;

}

const float PRECISION = 1000000.0;			//decimals points to consider
bool AIRoomba::getRandTrue(float chance){
	int threshold = chance * PRECISION;
	return (getRandInt(0, PRECISION) >= threshold ? true : false);
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





const float STEER_BUFFER_DELTA = 0.20f;

vec3 getForwardVector(quat q) 
{
	return vec3( 2 * (q.x * q.z + q.w * q.y), 
		2 * (q.y * q.x - q.w * q.x),
		1 - 2 * (q.x * q.x + q.y * q.y));
}

//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer,Entity* who, vec3 to){

	vec3 whoDir(0,0,0);


	whoDir = getForwardVector(who->getRotation());
	vec3 toDir =  (to - who->getPosition());


	toDir = glm::normalize(toDir);
	whoDir = glm::normalize(whoDir);

	vec3 diff = (toDir - whoDir);


	printf("whoDir Z %f Y %f X %f\n", whoDir.z, whoDir.y, whoDir.x);
	printf("toDir Z %f Y %f X %f\n", toDir.z, toDir.y, toDir.x);
	printf("diffDir Z %f Y %f X %f\n", diff.z, diff.y, diff.x);
	float negation = (toDir.x *toDir.z) > 0 ? -1.0 : 1.0;


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




const float AWARE_DISTANCE = 50.5f;


const int UPDATE_CHECK = 200;
const int STUCK_CHECK = 1000;

const char* actionList[] = {"roam", "new_action"};


//probabilities
const float WEAPON_SWITCH_CHANCE = 0.10f;




//gets a sublist of all the entities that are within the radius of ai roomba
//returned vector does not contain list of self.
static void getNearbyEntities(AIRoomba* self, vector<Entity*>* entityList, vector<Entity*>* nearbyBuffer){


	//check with all other entities
	for (unsigned int j = 0; j < entityList->size(); j++){

		Entity* curE = entityList->at(j);

		if ((strcmp(curE->getTag(), "airoomba") == 0)){ // || (strcmp(entityList[j]->getTag(), "airoomba") == 0)){
			//an ai, check if its not self
			AIRoomba* ai = (AIRoomba*) curE;

			if (ai->getID() != self->getID()){
				float entityDistance = getDistance(self->getPosition(), curE->getPosition());
				if (entityDistance <= AWARE_DISTANCE){
					//within field of chase

					//printf("dist %f\n", entityDistance);
					nearbyBuffer->push_back(curE);
				}
			}
		}
		else{
			//if not self
			float entityDistance = getDistance(self->getPosition(), curE->getPosition());

			if (entityDistance <= AWARE_DISTANCE){
				//within field of chase

				//printf("dist %f\n", entityDistance);
				nearbyBuffer->push_back(curE);
			}
		}
	}

}



int AIRoomba::UpdateAI(std::vector<Entity*>* entityList)
{

	//check with all other entities
	for (unsigned int j = 0; j < entityList->size(); j++){

		Entity* curAI = entityList->at(j);

		if ((strcmp(curAI->getTag(), "roomba") == 0)){// || (strcmp(entityList[j]->getTag(), "airoomba") == 0)){
			//if not self
			float entityDistance = getDistance(this->getPosition(), curAI->getPosition());

			if (entityDistance <= AWARE_DISTANCE){
				//within field of chase

				printf("dist %f\n", entityDistance);
				driveTowards(&control, this, curAI->getPosition());
				//printf("Will steer %s\n", aiControls[i]->steer >=0 ? "RIGHT" : "LEFT");

			}
		}
	}




	if (cycle >= UPDATE_CHECK){
		printf("CYCLE\n");

		if (strcmp(action, "roam") == 0){
			//roam around, change action when nearby something


			//first check if anything interesting is near by
			vector<Entity*>* nearby = new vector<Entity*>(); 
			getNearbyEntities(this, entityList, nearby);

			if(nearby->size() >=0){
				//something nearby

				for (int i =0; i < nearby->size() ; i++){

					Entity* curE = nearby->at(i);


					//powerup nearby, decide on getting it.
					if (strcmp(curE->getTag(), "powerup") == 0){
						
						Powerup* power = (Powerup*) curE;

						if (this->getPowerupFlag() == false){
							//no powerup go and take it
							targetEntity = curE;
						}
						else if (this->getPowerupType() != powerup->type){
							//different powerup than one we have currently

							//decide on switching
							if (getRandTrue(WEAPON_SWITCH_CHANCE) == true){
								targetEntity = curE;
							}
						}
					}
					else if ((strcmp(curE->getTag(), "roomba") == 0) || (strcmp(curE->getTag(), "powerup") == 0)){
						//ais, attack them
						
					}
				}
			
			}
			else{
				//nothing nearby keep driving
			}

			delete nearby;			//delete this sublist
		}
		else if(strcmp(action, "get_weapon") == 0){
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

		//now drive towards the target position
		
	}


	cycle++;


	driveTowards(&control, this, targetEntity->getPosition());
	


	return 0;
}



int AIRoomba::Update(){
	return Roomba::Update();
}