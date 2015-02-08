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


XINPUT_STATE state;
static unordered_map<int, void (*)()>* listeners[MAX_CONTROLLERS];



//dummy function
void dummy(){}


Controller::Controller()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));


	//prev packet info for controllers
	packetNum[MAX_CONTROLLERS];
	//listeners[MAX_CONTROLLERS];
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
		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{


			//check if state has changed since last time.
			if (packetNum[i] != state.dwPacketNumber)
			{
				packetNum[i] = state.dwPacketNumber;
				checkState = true;
			}
		}
		else{
			checkState = false;
		}


		if (checkState == true)
		{
			//Button inputs, Dpad, Thumb stick press, hard shoulder buttons
			//Button is held down when ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);

			//A, B, X, Y
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0)
			{
				//printf("A%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_A);

			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
			{
				//printf("B%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_B);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
			{
				//printf("X%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_X);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0)
			{
				//printf("Y%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_Y);
			}

			//===========
			//D-pad
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
			{
				//printf("D-Left%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_LEFT);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
			{
				//printf("D-Right%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_RIGHT);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
			{
				//printf("D-Down%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_DOWN);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
			{
				//printf("D-Up%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_DPAD_UP);
			}

			//===================
			//shoulder buttons
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
			{
				//printf("Sh-Left%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_LEFT_SHOULDER);
			}	
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
			{
				//printf("Sh-Right%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_RIGHT_SHOULDER);
			}

			//=======================
			//thumb buttons
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
			{
				//printf("Left Thumb%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_LEFT_THUMB);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
			{
				//printf("Right thumb%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_RIGHT_THUMB);
			}


			//=======================
			//back and start
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
			{
				//printf("Back%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_BACK);
			}
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
			{
				//printf("Start%i\n", i);
				emitButtonEvent(i, XINPUT_GAMEPAD_START);
			}

			//========================
			//trigger buttons, amount pressed from 0-255. Threshold for deadzone.
			if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				//printf("Left Trig%i: %i\n", i, state.Gamepad.bLeftTrigger);
			}

			if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				//printf("Right Trig%i: %i\n", i, state.Gamepad.bRightTrigger);
			}

			//=========================
			//analog sticks, amount in direction is between -32768 to 32767
			if ((abs(state.Gamepad.sThumbLX) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) || (abs(state.Gamepad.sThumbLY) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				//printf("Left ThumbX%i: %i\tLeft Thumb%iY: %i\n", i, state.Gamepad.sThumbLX, i, state.Gamepad.sThumbLY);
			}

			if ((abs(state.Gamepad.sThumbRX) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) || (abs(state.Gamepad.sThumbRY) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				//printf("Right ThumbX%i: %i\tRight Thumb%iY: %i\n", i, state.Gamepad.sThumbRX, i, state.Gamepad.sThumbRY);
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
		if (XInputGetState(i, &state) == ERROR_SUCCESS)
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


bool Controller::stateChanged()
{
	return checkState;
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





