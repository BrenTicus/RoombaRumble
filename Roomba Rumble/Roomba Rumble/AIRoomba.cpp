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

//returns true only 'chance' percent of the time. Use chance in the interval [0.0f, 1.0f]
bool AIRoomba::getRandTrue(float chance){
	int threshold = (1.0f - chance) * PRECISION;
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



//gets a sublist of all the entities that are within the radius of ai roomba
//returned vector does not contain list of self.
//Provide a filter tag to get list of entities that match the tag, leave empty string otherwise to get any entity type
//Calling this method on the same nearby buffer will add to the list of entities
static void getNearbyEntities(AIRoomba* self, vector<Entity*>* entityList, vector<Entity*>* nearbyBuffer, const float awarenessDist, const char* filterTag){

	bool useFilter = true;
	if (strcmp(filterTag, "") == 0){
		useFilter = false;
	}


	if (useFilter == false){

		//check with all other entities
		for (unsigned int j = 0; j < entityList->size(); j++){

			Entity* curE = entityList->at(j);

			if ((strcmp(curE->getTag(), "airoomba") == 0)){ 
				//an ai, check if its not self
				AIRoomba* ai = (AIRoomba*) curE;

				if (ai->getID() != self->getID()){
					float entityDistance = getDistance(self->getPosition(), curE->getPosition());
					if (entityDistance <= awarenessDist){
						//within field of chase

						nearbyBuffer->push_back(curE);
					}
				}
			}
			else{
				//if not self
				float entityDistance = getDistance(self->getPosition(), curE->getPosition());

				if (entityDistance <= awarenessDist){
					//within field of chase

					nearbyBuffer->push_back(curE);
				}
			}
		}
	}
	else if (useFilter == true){
		//use the filter to get only entity types specified

		//check with all other entities
		for (unsigned int j = 0; j < entityList->size(); j++){

			Entity* curE = entityList->at(j);

			if ((strcmp(curE->getTag(), "airoomba") == 0)){ 
				//an ai, check if its not self
				AIRoomba* ai = (AIRoomba*) curE;

				if (ai->getID() != self->getID()){
					float entityDistance = getDistance(self->getPosition(), curE->getPosition());
					if (entityDistance <= awarenessDist){
						//within field of chase

						if (strcmp(filterTag, curE->getTag()) == 0){
							//matches filter tag

							nearbyBuffer->push_back(curE);
						}
					}
				}
			}
			else{
				//if not self
				float entityDistance = getDistance(self->getPosition(), curE->getPosition());

				if (entityDistance <= awarenessDist){
					//within field of chase

					if (strcmp(filterTag, curE->getTag()) == 0){
						//matches filter tag

						nearbyBuffer->push_back(curE);
					}
				}
			}
		}
	}

}


//AI Tweaking constants
const float AWARE_DISTANCE = 50.0f;
const float ATTACK_AWARE_DISTANCE = AWARE_DISTANCE + 10.0f;

const int UPDATE_CHECK = 100;
const int STUCK_CHECK = 50;

const char* actionList[] = {"roam", "new_action"};



//probabilities
const float WEAPON_SWITCH_CHANCE = 0.010f;						//chance of switching powerups nearby if we already have powerup
const float NO_WEAPON_ATTACK_CHANCE = 0.30f;					//chance of attacking if we have powerups

int AIRoomba::UpdateAI(std::vector<Entity*>* entityList)
{


	if (cycle >= UPDATE_CHECK){
		

		if (strcmp(action, "roam") == 0){
			//roam around, change action when nearby something


			//first check if anything interesting is near by
			vector<Entity*>* nearbyPowerups = new vector<Entity*>(); 
			getNearbyEntities(this, entityList, nearbyPowerups, AWARE_DISTANCE, "powerup");

			vector<Entity*>* nearbyPlayers = new vector<Entity*>(); 
			getNearbyEntities(this, entityList, nearbyPlayers, AWARE_DISTANCE, "airoomba");
			getNearbyEntities(this, entityList, nearbyPlayers, AWARE_DISTANCE, "roomba");
			

			if((nearbyPowerups->size() > 0) && (nearbyPlayers->size() > 0)){
				//powerup nearby and player nearby

				//target random player if we have a weapon, otherwise go for weapon
				if (this->hasPowerup() == true){
					//we can fight random player
					targetEntity = nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1));
					action = "attack";
				}
				else{
					//go after weapon instead
					targetEntity = nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1));
				}

			}
			else if (nearbyPowerups->size() > 0){
				//powerup nearby
				if (this->hasPowerup() == false){
					//no weapon, equip self
					targetEntity = nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1));
				}
				else if (getRandTrue(WEAPON_SWITCH_CHANCE) == true){
					//we have a weapon already but wanna switch
					targetEntity = nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1));
				}
			}
			else if (nearbyPlayers->size() > 0){
				//nearby players
				if (this->hasPowerup() == true){
					//we can fight random player
					targetEntity = nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1));
					action = "attack";
				}
				else if (getRandTrue(NO_WEAPON_ATTACK_CHANCE)){ 
					//attack player without powerup
					targetEntity = nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1));
				}
			}
			else{
				//nothing nearby keep driving to somewhere
				targetEntity = (Entity*) this;
			}

			//delete this sublists
			delete nearbyPlayers;			
			delete nearbyPowerups;

		}
		else if (strcmp(action, "attack") == 0){
			//ATTACK!
			vector<Entity*>* nearbyPlayers = new vector<Entity*>(); 
			getNearbyEntities(this, entityList, nearbyPlayers, ATTACK_AWARE_DISTANCE, "roomba");

			if (nearbyPlayers->size() > 0){


				//check if old target still in range, if not choose another one in the same range.
				for( int i=0; i < nearbyPlayers->size(); i++){
					if (targetEntity != nearbyPlayers->at(i)){
						//old target gone. Try attacking new target
						targetEntity = nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1));
						break;
					}
				}
				
			}
			else{
				//no roombas left in area, roam
				targetEntity = (Entity*) this;
				action = "roam";
			}
			delete nearbyPlayers;
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


	driveTowards(control, this, targetEntity->getPosition());



	return 0;
}



int AIRoomba::Update(){
	return Roomba::Update();
}