

class Controller
{
private:
	int connectedControllers;
	int packetNum[];
public:
	Controller::Controller();
	Controller::~Controller();
	void Controller::update();
	void Controller::checkControllers();
};