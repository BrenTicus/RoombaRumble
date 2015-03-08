#pragma once

class Controller
{
private:
	bool checkState;
	int connectedControllers;
	int packetNum[];
	
	
public:
	Controller::Controller();
	Controller::~Controller();
	void Controller::update();
	void Controller::emitButtonEvent(int player, int event);
	void Controller::checkControllers();
	void Controller::registerButtonEvent(int button, int player, void (*callback)());


	bool Controller::stateChanged();

	//trigger values
	int Controller::getLeftTrigger(int player);
	int Controller::getRightTrigger(int player);
	
	//thumbstick values
	int Controller::getRightThumbX(int player);
	int Controller::getRightThumbY(int player);
	int Controller::getLeftThumbX(int player);
	int Controller::getLeftThumbY(int player);

	//==============
	//button pressed down

	//buttons
	bool Controller::getADown(int player);
	bool Controller::getBDown(int player);
	bool Controller::getXDown(int player);
	bool Controller::getYDown(int player);

	//D-pad pressed down
	bool Controller::getLeftDown(int player);
	bool Controller::getRightDown(int player);
	bool Controller::getDownDown(int player);
	bool Controller::getUpDown(int player);

	//Shoulder buttons pressed down
	bool Controller::getShLeftDown(int player);
	bool Controller::getShRightDown(int player);

	//Thumb pad pressed down
	bool Controller::getThumbLeftDown(int player);
	bool Controller::getThumbRightDown(int player);

	//Back or start pressed down
	//nah no need.

	void Controller::buttonToString(int buttonCode, int player);

};

struct Point{
	int x,y;

};