#include "Sound.h"
#include <string>


FMOD_RESULT result;
FMOD::System *soSystem;
FMOD::Sound *music;
FMOD::Sound *sound;

//For now just a simple file to play one ogg. Later we can potentially make an overarching sound class, or switch this to handle all sounds.
//Uses just FMOD LowLevel, but can be switched to FMOD Studio if we wish to use the features.

Sound::Sound(){
	result = FMOD::System_Create(&soSystem); //Initialize FMOD
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	result = soSystem->init(512, FMOD_INIT_NORMAL, 0);
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	channel->setVolume(0.3f);
	channel->setPaused(false);
}

void Sound::playMusic(std::string fileName){
	fileName = "./Assets/" + fileName;
	result = soSystem->createSound(fileName.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, 0, &music); //Select our BGM. For now, we use the test OGG.
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	result = soSystem->playSound(music, 0, true, &channel); //Play the BGM
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
	channel->setVolume(0.3f);
	channel->setPaused(false);
}

void Sound::playSound(std::string fileName){
	fileName = "./Assets/" + fileName;
	soSystem->createSound(fileName.c_str(), FMOD_DEFAULT, 0, &sound);
	soSystem->playSound(sound, 0, false, &channel);
}

void Sound::update(){ //Update FMOD every tick
	result = soSystem->update();
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
};