#include "Keyboard.h"
#include <iostream>


using namespace std;

static bool keyDown[350];



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (action == GLFW_REPEAT)
	{
		printf("HOLD: %i\n", key);
		keyDown[key] = true;

	}
	else if (action == GLFW_PRESS){
		printf("PRESS: %i\n", key);
		keyDown[key] = true;

	}
	else if (action == GLFW_RELEASE)
	{
		printf("RELEASE: %i\n", key);
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
	return false; //keyDown.at(key);
}









static int keyAccel = GLFW_KEY_W;
static int keyBraking = GLFW_KEY_SPACE;
static int keyLeft = GLFW_KEY_A;
static int keyRight = GLFW_KEY_D;
static int keyReverse = GLFW_KEY_S;
static int keyMenu = GLFW_KEY_MENU;
static int keyEBrake = GLFW_KEY_LEFT_SHIFT;
static int keyWeapon = GLFW_KEY_F;
static int keyCancel = GLFW_KEY_BACKSPACE;
static int keyAccept = GLFW_KEY_ENTER;






bool getAccel_Pressed(){ return keyDown[keyAccel]; }
bool getBraking_Pressed(){ return keyDown[keyBraking]; }
bool getLeftSteer_Pressed(){ return keyDown[keyLeft]; }
bool getRightSteer_Pressed(){ return keyDown[keyRight]; }
bool getReverse_Pressed(){ return keyDown[keyReverse]; }
bool getMenu_Pressed(){ return keyDown[keyMenu]; }
bool getEBrake_Pressed(){ return keyDown[keyEBrake]; }
bool getActivateWeapons_Pressed(){ return keyDown[keyWeapon]; }
bool getAccept_Pressed(){ return keyDown[keyAccept]; }
bool getCancel_Pressed(){ return keyDown[keyCancel]; }