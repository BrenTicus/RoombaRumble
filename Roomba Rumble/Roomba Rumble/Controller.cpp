#include "Controller.h"
#include <iostream>
#include <Windows.h>
#include <XInput.h>

//deadzone for analog stick
float deadzoneL = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
float deadzoneR = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;		


const int MAX_CONTROLLERS = 1;


XINPUT_STATE state;


Controller::Controller()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));


	//prev packet info for controllers
	packetNum[MAX_CONTROLLERS];
	ZeroMemory(&packetNum, sizeof(int) * MAX_CONTROLLERS);
		

	connectedControllers = 0;
	checkControllers();
}




void Controller::update(){

	

	bool checkState = false;
	

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
				printf("A%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0)
			{
				printf("B%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0)
			{
				printf("X%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0)
			{
				printf("Y%i\n", i);
			}

				
			//D-pad
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
			{
				printf("D-Left%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
			{
				printf("D-Right%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
			{
				printf("D-Down%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
			{
				printf("D-Up%i\n", i);
			}


			//shoulder buttons
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0)
			{
				printf("Sh-Left%i\n", i);
			}	
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0)
			{
				printf("Sh-Right%i\n", i);
			}


			//thumb buttons
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0)
			{
				printf("Left Thumb%i\n", i);
			}

			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0)
			{
				printf("Right thumb%i\n", i);
			}
						


			//back and start
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0)
			{
				printf("Back%i\n", i);
			}
			if((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0)
			{
				printf("Start%i\n", i);
			}


			//trigger buttons, amount pressed from 0-255. Threshold for deadzone.
			if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				printf("Left Trig%i: %i\n", i, state.Gamepad.bLeftTrigger);
			}

			if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				printf("Right Trig%i: %i\n", i, state.Gamepad.bRightTrigger);
			}


			//analog sticks, amount in direction is between -32768 to 32767
			if ((abs(state.Gamepad.sThumbLX) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) || (abs(state.Gamepad.sThumbLY) >  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
			{
				printf("Left ThumbX%i: %i\tLeft Thumb%iY: %i\n", i, state.Gamepad.sThumbLX, i, state.Gamepad.sThumbLY);
			}

			if ((abs(state.Gamepad.sThumbRX) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) || (abs(state.Gamepad.sThumbRY) >  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
			{
				printf("Right ThumbX%i: %i\tRight Thumb%iY: %i\n", i, state.Gamepad.sThumbRX, i, state.Gamepad.sThumbRY);
			}
		}
	}

}


//checks the controller state for newly added controllers,
//To increase max controllers to check see MAX_CONTROLLERS
void Controller::checkControllers(){

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