#include "Controller.h"
#include <iostream>
#include <Windows.h>
#include <XInput.h>
#include <unordered_map>

using namespace std;

//deadzone for analog stick
float deadzoneL = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
float deadzoneR = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;		

enum ButtonState{
	BUTTON_DOWN,
	BUTTON_HOLD,
	BUTTON_UP
};

const int MAX_CONTROLLERS = 1;


XINPUT_STATE state[MAX_CONTROLLERS];
static unordered_map<int, void (*)()>* listeners[MAX_CONTROLLERS];

Point rightThumb[MAX_CONTROLLERS];
Point leftThumb[MAX_CONTROLLERS];
int rightTrigger[MAX_CONTROLLERS];
int leftTrigger[MAX_CONTROLLERS];

//dummy function
void dummy(){}


Controller::Controller()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));


	//prev packet info for controllers
	packetNum[MAX_CONTROLLERS];
	ZeroMemory(&packetNum, sizeof(int) * MAX_CONTROLLERS);

	int input;
	unordered_map<int,void (*)()>* map;
	for (int i =0; i < MAX_CONTROLLERS; i++)
	{
		map = new unordered_map<int,void (*)()>();

		for (int j = 0; j < 16; j++)
		{
			map->emplace(1 << j, dummy);
		}

		listeners[i] = map;
	}


	connectedControllers = 0;
	checkControllers();
}




void Controller::update()
{



	checkState = false;


	//check state
	for (int i =0; i< connectedControllers; i++)
	{


		//see if previous state was changed
		checkState = false;
		if (XInputGetState(i, &state[i]) == ERROR_SUCCESS)
		{


			//check if state has changed since last time.
			if (packetNum[i] != state[i].dwPacketNumber)
			{
				packetNum[i] = state[i].dwPacketNumber;
				checkState = true;
			}
		}
		else{
			checkState = false;
		}


		if (checkState == true)
		{
			//Button inputs, Dpad, Thumb stick press, hard shoulder buttons
			//Button is held down when ((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);

			//A, B, X, Y
			if ((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_A);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_B);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_X);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_Y);
			}

			//===========
			//D-pad
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_LEFT);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_RIGHT);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_DOWN);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_UP);
			}

			//===================
			//shoulder buttons
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_LEFT_SHOULDER);
			}	
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_RIGHT_SHOULDER);
			}

			//=======================
			//thumb buttons
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_LEFT_THUMB);
			}

			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_RIGHT_THUMB);
			}


			//=======================
			//back and start
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_BACK);
			}
			if((state[i].Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
			{
				emitButtonEvent(i, XINPUT_GAMEPAD_START);
			}

			//========================
			//trigger buttons, amount pressed from 0-255. Threshold for deadzone.
			if (state[i].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				//printf("Left Trig%i: %i\n", i, state[i].Gamepad.bLeftTrigger);
				leftTrigger[i] = state[i].Gamepad.bLeftTrigger;
			}

			if (state[i].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				//printf("Right Trig%i: %i\n", i, state[i].Gamepad.bRightTrigger);
				rightTrigger[i] = state[i].Gamepad.bRightTrigger;
			}

			//=========================
			//analog sticks, amount in direction is between -32768 to 32767
			if ((abs(state[i].Gamepad.sThumbLX) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) || (abs(state[i].Gamepad.sThumbLY) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				//printf("Left ThumbX%i: %i\tLeft Thumb%iY: %i\n", i, state[i].Gamepad.sThumbLX, i, state[i].Gamepad.sThumbLY);
				leftThumb[i].x = state[i].Gamepad.sThumbLX;
				leftThumb[i].y = state[i].Gamepad.sThumbLY;

			}
			else
			{
				leftThumb[i].x = 0;
				leftThumb[i].y = 0;
			}
			if ((abs(state[i].Gamepad.sThumbRX) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) || (abs(state[i].Gamepad.sThumbRY) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				//printf("Right ThumbX%i: %i\tRight Thumb%iY: %i\n", i, state[i].Gamepad.sThumbRX, i, state[i].Gamepad.sThumbRY);
				rightThumb[i].x = state[i].Gamepad.sThumbRX;
				rightThumb[i].y = state[i].Gamepad.sThumbRY;
			}
			else
			{
				rightThumb[i].x = 0;
				rightThumb[i].y = 0;
			}
		}
	}

}



void Controller::emitButtonEvent(int player, int event)
{
	void (*callback)();
	callback = listeners[player]->at(event);
	callback();
	//buttonToString(event,player);				//for debug
}


//checks the controller state for newly added controllers,
//To increase max controllers to check see MAX_CONTROLLERS
void Controller::checkControllers()
{

	connectedControllers = 0;

	//check state of all 4 controllers
	for (int i =0; i< MAX_CONTROLLERS; i++)
	{
		if (XInputGetState(i, &state[i]) == ERROR_SUCCESS)
		{
			connectedControllers++;
		}
	}
}


//registers for a button event for a particular player. Players start at 0
//refer to XInput.H for button codes. Or otherwise look up in buttonToString
void Controller::registerButtonEvent(int button, int player, void (*callback)())
{
	listeners[player]->erase(button);
	listeners[player]->emplace(button, callback);
}


//checks if the state has changed
bool Controller::stateChanged()
{
	return checkState;
}

//trigger values
int Controller::getLeftTrigger(int player)
{
	return leftTrigger[player];
}
int Controller::getRightTrigger(int player)
{
	return rightTrigger[player];
}

//thumbstick values
int Controller::getRightThumbX(int player)
{
	return rightThumb[player].x;
}
int Controller::getRightThumbY(int player)
{
	return rightThumb[player].y;
}
int Controller::getLeftThumbX(int player)
{
	return leftThumb[player].x;
}
int Controller::getLeftThumbY(int player)
{
	return leftThumb[player].y;
}

void Controller::buttonToString(int buttonCode, int player)
{

	
	//A, B, X, Y
	switch(buttonCode)
	{
	case XINPUT_GAMEPAD_A:
		printf("A%i\n", player);
		break;
	case XINPUT_GAMEPAD_B:
		printf("B%i\n", player);
		break;
	case XINPUT_GAMEPAD_X:
		printf("X%i\n", player);
		break;
	case XINPUT_GAMEPAD_Y:
		printf("Y%i\n", player);
		break;

		//========================
		//D-pad
	case XINPUT_GAMEPAD_DPAD_LEFT:
		printf("D-Left%i\n", player);
		break;
	case XINPUT_GAMEPAD_DPAD_RIGHT:
		printf("D-Right%i\n", player);
		break;
	case XINPUT_GAMEPAD_DPAD_DOWN:
		printf("D-Down%i\n", player);
		break;
	case XINPUT_GAMEPAD_DPAD_UP:
		printf("D-Up%i\n", player);
		break;

		//=========================
		//shoulder buttons
	case XINPUT_GAMEPAD_LEFT_SHOULDER:
		printf("Sh-Left%i\n", player);
		break;
	case XINPUT_GAMEPAD_RIGHT_SHOULDER:
		printf("Sh-Right%i\n", player);
		break;

		//=================
		//thumb buttons
	case XINPUT_GAMEPAD_LEFT_THUMB:
		printf("Left Thumb%i\n", player);
		break;
	case XINPUT_GAMEPAD_RIGHT_THUMB:
		printf("Right thumb%i\n", player);
		break;


		//=================
		//back and start
	case XINPUT_GAMEPAD_BACK:
		printf("Back%i\n", player);
		break;
	case XINPUT_GAMEPAD_START:
		printf("Start%i\n", player);
		break;
	}
}





