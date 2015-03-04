#pragma once

#include "fmod.hpp"
#include "fmod_errors.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class Sound{
private:
	FMOD::Channel* channel;
	FMOD::Channel* soundChannel;
public:
		Sound();
		void update();
		void playMusic(string);
		void playSound(string);
};