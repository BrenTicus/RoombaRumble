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

int AIRoomba::Update()
{

	return Roomba::Update();
}