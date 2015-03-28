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

//given two position vectors, get the distance, the result is always non-negative
static float getDistance(vec3 pos1, vec3 pos2){
	vec3 diff = pos1 - pos2;
	return glm::sqrt(diff.x*diff.x + diff.y* diff.y + diff.z* diff.z);
}


const float ACCEL_DISTANCE_MAX = 20.0f;
const float ACCEL_MIN = 0.34;
//returns acceleration between 0.0 - 1.0 as one approaches
static float accelApproach(vec3 from, vec3 to){
	float distance = getDistance(from, to);
	float min = distance >= ACCEL_DISTANCE_MAX ? ACCEL_DISTANCE_MAX : distance;
	return glm::max(min / ACCEL_DISTANCE_MAX, ACCEL_MIN);
}





const float STEER_BUFFER_DELTA = 0.0f;


//Taken from: http://nic-gamedev.blogspot.ca/2011/11/quaternion-math-getting-local-axis.html?m=1
vec3 getForwardVector(quat q) 
{
	return vec3( 2 * (q.x * q.z + q.w * q.y), 
		2 * (q.y * q.x - q.w * q.x),
		1 - 2 * (q.x * q.x + q.y * q.y));
}


void AIRoomba::setTarget(Entity* targ){
	targetEntity = targ;
	targetPos = vec3(0,0,0);
}

void AIRoomba::setTarget(vec3 targ){
	targetEntity = NULL;
	targetPos = targ;
}

vec3 AIRoomba::getTargetPos(){
	if(targetEntity == NULL){
		return targetPos;
	}
	return targetEntity->getPosition();
}




//Sets the appropriate controls to get "who" to some entity "to"
void driveTowards(DriveControl* buffer,Entity* who, vec3 to, bool reverse){

	int reverseNeg = (reverse == false) ? 1: -1;

	vec3 whoDir(0,0,0);

	//forward direction of roomba
	whoDir = getForwardVector(who->getRotation());
	vec3 toDir =  (to - who->getPosition());


	//ignore height
	toDir.y = 0;
	whoDir.y = 0;


	toDir = glm::normalize(toDir);
	whoDir = glm::normalize(whoDir);

	vec3 diff = (toDir - whoDir);
	diff = diff*who->getRotation();
	diff = glm::normalize(diff);

	//printf("whoDir Z %f Y %f X %f\n", whoDir.z, whoDir.y, whoDir.x);
	//printf("toDir Z %f Y %f X %f\n", toDir.z, toDir.y, toDir.x);
	//printf("diffDir Z %f Y %f X %f\n", diff.z, diff.y, diff.x);
	float negation = ((diff.x) > 0.0f ? 1.0f : -1.0f) * reverseNeg;


	if ((diff.z >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.z <= (1.0 * STEER_BUFFER_DELTA)) && (diff.x >= (-1.0f*STEER_BUFFER_DELTA)) && (diff.x <= (1.0 * STEER_BUFFER_DELTA))){
		//printf("DRIVE STRAIGHT\n");
		buffer->steer = 0.0f;
	}
	else{
		buffer->steer = 0.8f * negation;
	}


	//check if drive backwards
	if (reverse == false){
		buffer->accel = accelApproach(who->getPosition(), to);
	}
	else{
		buffer->accel = -1.00;
	}
	//buffer->accel = 0.0f;
	buffer->braking = 0.0;

	//printf(buffer->steer >=0.0 ? "Steer RIGHT\n" : "Steer LEFT\n");
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

					if ((useFilter == true) && strcmp(filterTag, curE->getTag()) == 0){
						//matches filter tag

						nearbyBuffer->push_back(curE);
					}
					else {
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

				if (useFilter == true){


					if(strcmp(filterTag, curE->getTag()) == 0){
						nearbyBuffer->push_back(curE);
					}
				}
				else {
					nearbyBuffer->push_back(curE);
				}

			}
		}
	}


}


const float ROAM_X_MAX = 20.0f;
const float ROAM_X_MIN = -20.0f;
const float ROAM_Z_MAX = 20.f;
const float ROAM_Z_MIN = -20.0;

//gets a new random roam position
vec3 AIRoomba::getRandRoam(){
	vec3 newRoam;
	newRoam.x = getRandInt(ROAM_X_MIN, ROAM_X_MAX);
	newRoam.z = getRandInt(ROAM_Z_MIN, ROAM_Z_MAX);

	return newRoam;
}


//AI Tweaking constants
const float AWARE_DISTANCE = 15.0f;									//awareness radial distance of other objects around AI
const float ATTACK_AWARE_DISTANCE = AWARE_DISTANCE + 15.0f;			//distance to stay on a player's trail for attack

const int UPDATE_CHECK = 50;					//state update check frequency

const int STUCK_CHECK = 25;						//frequency to check when stuck
const int ESCAPE_THRESHOLD = 10;				//number of stuck checks before deciding to escape stuckness
const float STUCK_DISTANCE_DELTA = 1.0f;		//Stuckness delta


const float BACKUP_DISTANCE = 7.0f;				//how far to back up to set up escape point (goal)
const float ESCAPED_POSITION_DISTANCE = 4.5f;	//how far to back up from old position

const float ROAM_APPROACH = 3.0f;				//how far to roam towards point before choosing new roam

//probabilities
const float WEAPON_SWITCH_CHANCE = 0.010f;						//chance of switching powerups nearby if we already have powerup
const float NO_WEAPON_ATTACK_CHANCE = 0.30f;					//chance of attacking if we have powerups

const float WEAPON_CONFIDENCE_CHANCE = 0.20f;					//decreased chance of running away if we have a powerup
const float LOW_HEALTH_WARNING = 1.0f;							//the health points difference from max to start worrying about escaping


void AIRoomba::State_Roam(std::vector<Entity*>* entityList){
	static const int ROAM_CYCLE_THRESHOLD = 20;

	if (cycle >= ROAM_CYCLE_THRESHOLD){
		//roam around, change action when nearby something

		printf("STATE: ROAM\n");
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
				setTarget(nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1)));
				stateFunc = &AIRoomba::State_Attack;
			}
			else{
				//go after weapon instead
				setTarget(nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1)));
				stateFunc = &AIRoomba::State_GetItem;
			}

		}
		else if (nearbyPowerups->size() > 0){
			//powerup nearby
			if (this->hasPowerup() == false){
				//no weapon, equip self
				setTarget(nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1)));
				stateFunc = &AIRoomba::State_GetItem;
			}
			else if (this->hasPowerup() == true){
				//we already have a powerup, can we upgrade?
				Entity* matchingPowerup = NULL;

				Powerup* cursor = NULL;

				//find a matching powerup type.
				for (int i =0 ; i < nearbyPowerups->size(); i++){
					cursor = (Powerup*) nearbyPowerups->at(i);
					if (this->getPowerupType() == cursor->getType()){
						matchingPowerup = nearbyPowerups->at(i);
					}

				}


				if (matchingPowerup == NULL){
					//no matching upgrade, switch randomly

					if(getRandTrue(WEAPON_SWITCH_CHANCE)){
						//we have a weapon already but wanna switch
						setTarget(nearbyPowerups->at(getRandInt(0, nearbyPowerups->size()-1)));
						stateFunc = &AIRoomba::State_GetItem;
					}
				}
				else{
					// there's a matching powerup, go and pickup
					setTarget(matchingPowerup);
					stateFunc = &AIRoomba::State_GetItem;
				}


			}
		}
		else if (nearbyPlayers->size() > 0){
			//nearby players
			if (this->hasPowerup() == true){
				//we can fight random player
				setTarget(nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1)));
				stateFunc = &AIRoomba::State_Attack;
			}
			else if (getRandTrue(NO_WEAPON_ATTACK_CHANCE)){ 
				//attack player without powerup
				setTarget(nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1)));
				stateFunc = &AIRoomba::State_GetItem;
			}
		}
		else{
			//nothing nearby keep driving to somewhere

			float distance = getDistance(this->getPosition(), getTargetPos());
			if (distance <= ROAM_APPROACH){
				//once reached to somewhere we choose new roam path
				setTarget(getRandRoam());
			}
		}

		//delete this sublists
		delete nearbyPlayers;			
		delete nearbyPowerups;

		cycle = 0;
	}

	cycle++;



	driveTowards(control, this, getTargetPos(), false);


}

static const float ITEM_APPROACH = 3.0f;		//how far to reach within radius to have "gotten" the item

//assumes we are chasing an entity
void AIRoomba::State_GetItem(std::vector<Entity*>*){
	static const int GET_ITEM_CYCLE_THRESHOLD = 10;

	if (cycle >= GET_ITEM_CYCLE_THRESHOLD){
		//Check if we are at the point 
		printf("STATE: GET_ITEM\n");

		float distance = getDistance(this->getPosition(), getTargetPos());


		if (distance < ITEM_APPROACH){
			//we have arrived, now go back to roam.

			stateFunc = &AIRoomba::State_Roam;
			setTarget(getRandRoam());
		}
		cycle = 0;
	}

	cycle++;


	driveTowards(control, this, getTargetPos(), false);
}

void AIRoomba::State_Attack(std::vector<Entity*>* entityList){
	static const int ATTACK_CYCLE_THRESHOLD = 10;

	if (cycle >= ATTACK_CYCLE_THRESHOLD){
		//ATTACK!
		printf("STATE: ATTACK\n");
		vector<Entity*>* nearbyPlayers = new vector<Entity*>(); 
		getNearbyEntities(this, entityList, nearbyPlayers, ATTACK_AWARE_DISTANCE, "roomba");
		getNearbyEntities(this, entityList, nearbyPlayers, ATTACK_AWARE_DISTANCE, "airoomba");

		if (nearbyPlayers->size() > 0){

			//probability of escaping when low on health
			//if we have a weapon, the chance is lowered
			float escapeProb = (1.0f - (this->getHealth() / (this->getMaxHealth() - LOW_HEALTH_WARNING)) - (this->hasPowerup() == true ? (WEAPON_CONFIDENCE_CHANCE) : 0.0f) );


			if (getRandTrue(escapeProb)){
				//we are weak on health, attack relutance, escape all roombas
				stateFunc = &AIRoomba::State_Escape;
				setTarget(getRandRoam());
			}
			else{

				bool found = false;
				//check if old target still in range, if not choose another one in the same range.
				for( int i=0; i < nearbyPlayers->size(); i++){
					if (targetEntity == nearbyPlayers->at(i)){
						//target still in range
						found = true;
						break;
					}
				}

				if (found == false){
					//old target out of range pick new
					setTarget(nearbyPlayers->at(getRandInt(0, nearbyPlayers->size()-1)));
				}
				else{
					//player still in range
				}
			}
		}
		else{
			//no roombas left in area, roam
			setTarget(getRandRoam());
			cycle = 0;
			stateFunc = &AIRoomba::State_Roam;
		}
		delete nearbyPlayers;

		cycle = 0;
	}



	cycle++;


	driveTowards(control, this, getTargetPos(), false);
	control->shooting = true;


}



void AIRoomba::State_Escape(std::vector<Entity*>* entityList){
	static const int ESCAPE_CYCLE_THRESHOLD = 10;

	if (cycle >= ESCAPE_CYCLE_THRESHOLD){
		//escape the all roombas
		printf("STATE: RUN AWAY\n");
		vector<Entity*>* nearbyPlayers = new vector<Entity*>(); 
		getNearbyEntities(this, entityList, nearbyPlayers, ATTACK_AWARE_DISTANCE, "roomba");
		getNearbyEntities(this, entityList, nearbyPlayers, ATTACK_AWARE_DISTANCE, "airoomba");

		if (nearbyPlayers->size() > 0){
			//keep escaping
			float distance = getDistance(this->getPosition(), getTargetPos());

			if (distance < ROAM_APPROACH){
				//at destination, pick new location to roam to
				setTarget(getRandRoam());
			}
		}
		else{
			//no roombas left in area, roam
			setTarget(getRandRoam());
			cycle = 0;
			stateFunc = &AIRoomba::State_Roam;
		}
		delete nearbyPlayers;

		cycle = 0;
	}


	cycle++;


	driveTowards(control, this, getTargetPos(), false);
}


void AIRoomba::State_EscapeStuck(std::vector<Entity*>* entityList){
	static const int ESCAPE_STUCK_CYCLE_THRESHOLD = 20;

	if (cycle >= ESCAPE_STUCK_CYCLE_THRESHOLD){
		//actively attempt to reverse
		printf("STATE: ESCAPE STUCK\n");
		driveTowards(control, this, targetPos, true);

		float distance = getDistance(this->getPosition(), revOldPosition);
		//printf("BACKUP DIST=%d::%f\n", this->getID(), distance);
		if (distance >= ESCAPED_POSITION_DISTANCE){
			stateFunc = &AIRoomba::State_Roam;		//start in roam state
			setTarget(getRandRoam());
			cycle = UPDATE_CHECK;	//check immediately next update
		}

		cycle = 0;
	}


	cycle++;


	driveTowards(control, this, getTargetPos(), true);


}







int AIRoomba::UpdateAI(std::vector<Entity*>* entityList)
{

	(this->*stateFunc)(entityList);


	//check if our position hasnt changed for a while, then switch to escape mode
	if((stuckCycle >= STUCK_CHECK) && (this->stateFunc != &AIRoomba::State_EscapeStuck)){

		float changeDistance = getDistance(this->getPosition(), lastPosition);

		if (changeDistance >= STUCK_DISTANCE_DELTA){
			//we have been able to escape, after some time resume regular activites
			stuckCycleCount = 0;

		}
		else{
			//we are still stuck
			stuckCycleCount++;
			if(stuckCycleCount >= ESCAPE_THRESHOLD){
				//stuck for a while. attempt backup and escape.
				stuckCycleCount=0;

				//switch modes and choose new position to escape to
				stateFunc = &AIRoomba::State_EscapeStuck;
				vec3 carDirection = getForwardVector(this->getRotation()) * -1.0f;
				carDirection = glm::normalize(carDirection);
				carDirection.x = getRandTrue(0.5) ? 1.0f : -1.0f;			//backup to left or right randomly
				carDirection = carDirection*this->getRotation();
				carDirection = glm::normalize(carDirection);
				setTarget(this->getPosition() + (BACKUP_DISTANCE * carDirection));
				revOldPosition = this->getPosition();
			}

		}


		lastPosition = this->getPosition();
		stuckCycle = 0;
	}
	stuckCycle = (stuckCycle + 1) % (STUCK_CHECK + 1);






	/*
	//Target only the player (debugging)
	Entity* cursor;
	for (int i =0 ; i < entityList->size(); i++){
	cursor = entityList->at(i);
	if (strcmp(cursor->getTag(), "roomba") == 0){
	driveTowards(this->control, this, entityList->at(i)->getPosition(), false);
	}

	}
	*/
	return 0;
}



int AIRoomba::Update(){
	return Roomba::Update();
}