#include "Sound.h"

FMOD_RESULT result;
FMOD::System *soSystem;
FMOD::Sound *music;
FMOD::Sound *sound;


static FMOD_VECTOR* convertVec3ToFmodVec(glm::vec3 position){
	FMOD_VECTOR* convert = new FMOD_VECTOR();
	convert->x = position.x;
	convert->y = position.y;
	convert->z = position.z;
	return convert;
}




//For now just a simple file to play one ogg. Later we can potentially make an overarching sound class, or switch this to handle all sounds.
//Uses just FMOD LowLevel, but can be switched to FMOD Studio if we wish to use the features.

Sound* Sound::mainSound = NULL;

Sound::Sound(){
	if (!mainSound) mainSound = this;
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
}

Sound::~Sound()
{
	if (mainSound == this) mainSound = NULL;
	soSystem->close();
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

void Sound::playSound(std::string fileName, glm::vec3 position){
	fileName = "./Assets/" + fileName;
	soSystem->createSound(fileName.c_str(), FMOD_3D, 0, &sound);
	channel->setPaused(true);
	channel->set3DAttributes(convertVec3ToFmodVec(position), 0);
	channel->setPaused(false);
	soSystem->playSound(sound, 0, false, &channel);
}

void Sound::update(glm::vec3 listenPosition, glm::vec3 listenVelocity, glm::vec3 listenForward, glm::vec3 listenUp){ //Update FMOD every tick

	soSystem->set3DListenerAttributes(0,convertVec3ToFmodVec(listenPosition), convertVec3ToFmodVec(listenVelocity), convertVec3ToFmodVec(listenForward), convertVec3ToFmodVec(listenUp));

	result = soSystem->update();
	if(result!= FMOD_OK){
		printf("FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}
};

