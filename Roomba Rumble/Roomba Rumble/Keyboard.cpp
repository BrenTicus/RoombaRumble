#include "Keyboard.h"
#include <iostream>


using namespace std;
const int TOTAL_KEYS = 350;					//total keys referenced in glfw
static bool keyDown[TOTAL_KEYS];



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (action == GLFW_REPEAT)
	{
		//printf("HOLD: %i\n", key);
		keyDown[key] = true;

	}
	else if (action == GLFW_PRESS){
		//printf("PRESS: %i\n", key);
		keyDown[key] = true;

	}
	else if (action == GLFW_RELEASE)
	{
		//printf("RELEASE: %i\n", key);
		keyDown[key] = false;

	}
	
}

Keyboard::Keyboard(GLFWwindow* keyWindow)
{
	
	window = keyWindow;
	glfwSetKeyCallback(window, key_callback);
	/*
	//alphabet characters
	for (int i=0; i < 25 ; i++)
	{
		keyDown.emplace('a' + i, false);
	}

	// , - . / and numeric characters
	for (int i=0; i < 14 ; i++)
	{
		keyDown.emplace(',' + i, false);
	}

	// [ \ ] ^ _ `    characters
	for (int i=0; i < 6 ; i++)
	{
		keyDown.emplace('[' + i, false);
	}

	keyDown.emplace(';', false);
	keyDown.emplace(' ', false);
	*/
}


Keyboard::~Keyboard(void)
{
}


//Gets if a key was pressed. Uses GLFW keycodes. Refer to glfw3.h for constants
bool Keyboard::getKeyPressed(int key)
{
	return keyDown[key];
}


	


bool Keyboard::getAccel_Pressed(){ return keyDown[keyAccel]; }
bool Keyboard::getBraking_Pressed(){ return keyDown[keyBraking]; }
bool Keyboard::getLeftSteer_Pressed(){ return keyDown[keyLeft]; }
bool Keyboard::getRightSteer_Pressed(){ return keyDown[keyRight]; }
bool Keyboard::getReverse_Pressed(){ return keyDown[keyReverse]; }
bool Keyboard::getMenu_Pressed(){ return keyDown[keyMenu]; }
bool Keyboard::getEBrake_Pressed(){ return keyDown[keyEBrake]; }
bool Keyboard::getActivateWeapons_Pressed(){ return keyDown[keyWeapon]; }
bool Keyboard::getAccept_Pressed(){ return keyDown[keyAccept]; }
bool Keyboard::getCancel_Pressed(){ return keyDown[keyCancel]; }