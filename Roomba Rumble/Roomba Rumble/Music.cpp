#include "Music.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <stdio.h>
#include <stdlib.h>

FMOD_RESULT result;
FMOD::System *music;
FMOD::Sound *sound;

//For now just a simple file to play one ogg. Later we can potentially make an overarching sound class, or switch this to handle all sounds.
//Uses just FMOD LowLevel, but can be switched to FMOD Studio if we wish to use the features.

Music::Music(){
	result = FMOD::System_Create(&music); //Initialize FMOD
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	result = music->init(512, FMOD_INIT_NORMAL, 0);
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}


	result = music->createSound("./Assets/Kick_their_ass.ogg", FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound); //Select our BGM. For now, we use the test OGG.
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}


	result = music->playSound(sound, 0, false, 0); //Play the BGM
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
}

void Music::update(){ //Update FMOD every tick
	result = music->update();
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
};