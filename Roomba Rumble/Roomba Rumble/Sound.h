#pragma once

#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <glm\geometric.hpp>

using namespace std;

class Sound{
private:
	FMOD::Channel* channel;
	FMOD::Channel* soundChannel;
public:
		Sound();
		void update();
		void update(glm::vec3 listenPosition, glm::vec3 listenVelocity, glm::vec3 listenForward, glm::vec3 listenUp);
		void playMusic(string);
		void playSound(string);
		void Sound::playSound(std::string filename, glm::vec3 position);
};