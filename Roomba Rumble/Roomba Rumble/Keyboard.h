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

