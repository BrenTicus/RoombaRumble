#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Sound.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>
#include "game.h"

static Game::GameState _gameState;
Controller* control;
Renderer* renderer;
PhysicsManager* physicsManager;
ResourceManager* resourceManager;
EntityManager* entityManager;
Keyboard* keyboard;
Sound* sound;

const int wWidth = 1600;
const int wHeight = 900;
int menuNum = 0;
int pos = 0;
bool winnerFlag;
float lastPress = 0.0f;

const std::string CONFIG_FILE = "game_config"; //for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble
const int GAME_TIME = 180;
const int BUTTON_COOLDOWN = 0.25f * CLOCKS_PER_SEC;

// Set up major systems.
int initialize()
{
	winnerFlag = false;
	if(!resourceManager) resourceManager = new ResourceManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager();
	renderer = new Renderer(GAME_TIME, wWidth, wHeight);
	keyboard = Keyboard::getInstance(renderer->getWindow());			//keyboard does not need updating, singleton
	sound = new Sound();
	physicsManager -> sound = sound;
	entityManager -> sound = sound;
	sound ->playMusic("Kick_their_ass.ogg");

	control = entityManager->getController();


	return 0;
}

void shutdown()
{
	delete sound;
	delete renderer;
	delete entityManager;
	delete physicsManager;
}

void restartGame();

void menu_pressedA()
{
	if (clock() - lastPress < BUTTON_COOLDOWN) return;
	lastPress = clock();

	printf("PRESSED A\n");
	if (_gameState == Game::GameState::Menu)
	{
		//Selecting a Menu Option
		if(pos == 0)
		{
			_gameState = Game::GameState::Playing;
		}
		else if(pos == 1)
		{
			_gameState = Game::GameState::Instructions;
		}
		else if(pos == 2)
		{
			_gameState = Game::GameState::Exiting;
		}
	}
	else if (_gameState == Game::GameState::Paused)
	{
		//pause menu

		if(pos == 0)
		{
			_gameState = Game::GameState::Playing;
		}
		else if(pos == 1)
		{
			_gameState = Game::GameState::InstructionsPaused;
		}
		else if(pos == 2)
		{
			_gameState = Game::GameState::Menu;
			restartGame();
		}
		else if(pos == 3)
		{
			_gameState = Game::GameState::Exiting;
		}
	}
	else if (_gameState == Game::GameState::Final)
	{
		
		if(pos == 0)
		{
			
			_gameState = Game::GameState::Playing;
			restartGame();
		}
		if(pos == 1)
		{
			_gameState = Game::GameState::Menu;
			restartGame();
		}
		if(pos == 2)
		{
			_gameState = Game::GameState::Exiting;
		}
	}
	else if(_gameState == Game::GameState::Instructions)
	{
		_gameState = Game::GameState::Menu;
	}
	else if(_gameState == Game::GameState::InstructionsPaused)
	{
		_gameState = Game::GameState::Paused;
	}

}

void menu_pressedB()
{
	if (clock() - lastPress < BUTTON_COOLDOWN) return;
	lastPress = clock();

	printf("PRESSED B\n");
	if (_gameState == Game::GameState::Menu)
	{
		//main menu state
		if(menuNum == 2)
		{
			_gameState = Game::GameState::Exiting;
		}
	}
	else if (_gameState == Game::GameState::Paused)
	{
		//pause menu state
		if (menuNum == 0)
		{
			_gameState = Game::GameState::Playing;
		}
	}
	else if(_gameState == Game::GameState::Instructions)
	{
		_gameState = Game::GameState::Menu;
	}
	else if(_gameState == Game::GameState::InstructionsPaused)
	{
		_gameState = Game::GameState::Paused;
	}
}


int MAX_POSITIONS = 3;

void menu_pressedUp()
{
	if (clock() - lastPress < BUTTON_COOLDOWN) return;
	lastPress = clock();
	//menu pressed up
	pos = ((pos -1) + MAX_POSITIONS) % MAX_POSITIONS;
	printf("PRESSED UP%d\n", pos);
}

void menu_pressedDown()
{
	if (clock() - lastPress < BUTTON_COOLDOWN) return;
	lastPress = clock();
	//menu pressed down
	pos = (pos + 1) % MAX_POSITIONS;
	printf("PRESSED DOWN%d\n", pos);
}


void menu_pressedStart()
{
	if (clock() - lastPress < BUTTON_COOLDOWN) return;
	lastPress = clock();

	if (_gameState == Game::GameState::Playing){
		_gameState = Game::GameState::Paused;
	}
	else if (_gameState == Game::GameState::Paused){
		_gameState = Game::GameState::Playing;
	}
}

//makes a new instance of the game
void restartGame(){
	shutdown();
	initialize();

	control->registerButtonEvent(0x1000, 0, menu_pressedA);
	control->registerButtonEvent(0x2000, 0, menu_pressedB);
	control->registerButtonEvent(0x0001, 0, menu_pressedUp);
	control->registerButtonEvent(0x0002, 0, menu_pressedDown);
	control->registerButtonEvent(0x0010, 0, menu_pressedStart);
}

bool release;
bool pressedUp;
const int MENU_THUMB_DEADZONE = 12000;			// should not be less that the default deadzones in xinput.h
//Moving the highlight using thumbstick or keyboard
void menu_thumbUpdate()
{


	//Moving the highlight using thumbstick or keyboard
	if ((control->getLeftThumbY(0) >= MENU_THUMB_DEADZONE) || (keyboard->getAccel_Pressed() == true))
	{
		release = true;
		pressedUp = true;

	}
	else if ((control->getLeftThumbY(0) <= (-1*MENU_THUMB_DEADZONE)) || (keyboard->getBraking_Pressed() == true))
	{
		release = true;
		pressedUp = false;

	}
	else
	{
		//thumbpad released, activate event
		if ( release == true){
			release = false;
			if (pressedUp == true){
				menu_pressedUp();
			}
			else{
				menu_pressedDown();
			}
		}
	}

	//printf("%d\n", control->getLeftThumbY(0));
}

void ShowMenu()
{
	//set highlight bar under Play & pos = 0
	MAX_POSITIONS = 3;
	static vec3 highlightLocations[] = {vec3(0,0,0), vec3(0,0,0)};
	renderer->menu(0, pos);

	menu_thumbUpdate();

	control->update();

}

void ShowPause()
{
	//set highlight bar under Play & pos = 0
	MAX_POSITIONS = 4;
	renderer->menu(1, pos);

	menu_thumbUpdate();

	control->update();
}

void ShowFinal()
{
	//set highlight bar under Play & pos = 0
	MAX_POSITIONS = 3;

	renderer->menu(2, pos);

	menu_thumbUpdate();

	control->update();
}

void ShowInstructions()
{
	//Display Instructions Image
	MAX_POSITIONS = 1;

	renderer->menu(3, pos);

	menu_thumbUpdate();

	control->update();;
}

void Playing(){
	float lastRespawn = (float)clock();
	float lastAIUpdate = (float)clock();
	float lastAIRespawn = (float)clock();

	Camera* cam = renderer->getCamera();



	while (_gameState == Game::GameState::Playing)
	{
		physicsManager->Update();	// Do physics updates
		renderer->Update();				// Draw stuff
		entityManager->LateUpdate();	// Clean up entities from last iteration.
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities


		sound->update(cam->getPosition(), cam->getVelocity(), cam->getForward(), cam->getUp());


		if(clock() - lastAIUpdate > AI_UPDATE_COOLDOWN){
			entityManager->UpdateAI();
			lastAIUpdate = (float)clock();
		}

		if(clock() - lastRespawn > POWERUP_RESPAWN_COOLDOWN)
		{
			entityManager->respawnPowerups();
			lastRespawn = (float)clock();
		}

		if(clock() - lastAIRespawn > SPAWN_AI_COOLDOWN)
		{
			entityManager->respawnRoombas();
			lastAIRespawn = (float)clock();
		}

		/*
		//reload game constants from config on key 'o' (oh)
		if (keyboard->getKeyPressed(79) == true){
		static SettingsFile file(CONFIG_FILE);
		if (file.reloadFile() == true){
		printf("Config file reloaded\n");
		printf("%i\n", file.getInt("bot-count"));

		//SettingsFile::printWorkingDir();
		}
		}
		*/
		//print player location on key 'p'
		if (keyboard->getKeyPressed(80) == true){
			entityManager->printPlayerLocation();
		}


		if(!winnerFlag && renderer->getGameOver() == 1)
		{
			//win
			sound->playSound("you_win.wav");
			winnerFlag= true;
			_gameState = Game::Final;
		}
		else if(!winnerFlag && renderer->getGameOver() == 2)
		{
			//lose
			sound->playSound("you_lose.wav");
			winnerFlag = true;
			_gameState = Game::Final;
		}


	}
}

// Main game loop.
int gameLoop()
{
	control->registerButtonEvent(0x1000, 0, menu_pressedA);
	control->registerButtonEvent(0x2000, 0, menu_pressedB);
	control->registerButtonEvent(0x0001, 0, menu_pressedUp);
	control->registerButtonEvent(0x0002, 0, menu_pressedDown);
	control->registerButtonEvent(0x0010, 0, menu_pressedStart);

	_gameState = Game::GameState::Menu;

	Game::GameState previousState = _gameState;

	while(_gameState!=Game::GameState::Exiting)
	{

		
		if (_gameState != previousState){
			//menu changed. reset position
			pos = 0;
		}

		previousState = _gameState;

		switch(_gameState)
		{
		case Game::GameState::Menu : 
			{
				ShowMenu();
				break;
			}
		case Game::GameState::Paused :
			{
				ShowPause();
				break;
			}
		case Game::GameState::Final :
			{	
				ShowFinal();
				break;
			}
		case Game::GameState::Instructions :
			{
				ShowInstructions();
				break;
			}
		case Game::GameState::InstructionsPaused :
			{
				ShowInstructions();
				break;
			}
		case Game::GameState::Playing :
			{
				Playing();
				break;
			}

		}	
	}
	return 0;
}

// Program entry point.
int main()
{
	
	initialize();
	gameLoop();
	renderer->killWindow();
	shutdown();

	return 0;
}
