

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



	void Controller::buttonToString(int buttonCode, int player);

};

struct Point{
	int x,y;

};