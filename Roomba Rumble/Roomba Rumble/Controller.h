

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


	void Controller::buttonToString(int buttonCode, int player);

};

