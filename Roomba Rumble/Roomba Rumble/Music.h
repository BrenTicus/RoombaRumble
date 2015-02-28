#pragma once

#include "fmod.hpp"
#include "fmod_errors.h"
#include <stdio.h>
#include <stdlib.h>

class Music{
private:
	FMOD::Channel* channel;
public:
		Music();
		void update();
};