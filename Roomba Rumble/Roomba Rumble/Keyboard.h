#pragma once
#include <unordered_map>
#include "GLFW\glfw3.h"
using namespace std;

class Keyboard
{
private:

	static Keyboard* instance;
	//unordered_map<char,bool> keyDown;
	GLFWwindow* window;
	Keyboard(GLFWwindow* window);

	static const int keyAccel = GLFW_KEY_W;
	static const int keyBraking = GLFW_KEY_SPACE;
	static const int keyLeft = GLFW_KEY_A;
	static const int keyRight = GLFW_KEY_D;
	static const int keyReverse = GLFW_KEY_S;
	static const int keyMenu = GLFW_KEY_MENU;
	static const int keyEBrake = GLFW_KEY_LEFT_SHIFT;
	static const int keyWeapon = GLFW_KEY_F;
	static const int keyCancel = GLFW_KEY_BACKSPACE;
	static const int keyAccept = GLFW_KEY_ENTER;
public:

	//singleton
	static Keyboard* getInstance(GLFWwindow* window){
		static Keyboard instance(window);

		return &instance;
	}

	~Keyboard(void);
	bool Keyboard::getKeyPressed(int);

	void Keyboard::Update();

	bool getAccel_Pressed();
	bool getBraking_Pressed();
	bool getLeftSteer_Pressed();
	bool getRightSteer_Pressed();
	bool getReverse_Pressed();
	bool getMenu_Pressed();
	bool getEBrake_Pressed();
	bool getActivateWeapons_Pressed();
	bool getAccept_Pressed();
	bool getCancel_Pressed();


};

