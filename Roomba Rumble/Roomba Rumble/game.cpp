#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Controller.h"
#include "Music.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include "AIManager.h"
#include <iostream>
#include <vector>
Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
AIManager* aiManager;
EntityManager* entityManager;
Keyboard* keyboard;
Controller* control;
Music* music;


const std::string CONFIG_FILE = "game_config";									//for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble
const int AI_BOTS = 1;
const int NUM_PLAYERS = 1;
// Set up major systems.
int initialize()
{
	control = new Controller();
	inputManager = new InputManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager(physicsManager, AI_BOTS, NUM_PLAYERS);
	aiManager = new AIManager(entityManager->getPlayerRoombas(), entityManager->getAIRoombas());
	renderer = new Renderer(entityManager);
	keyboard = Keyboard::getInstance(renderer->getWindow());			//keyboard does not need updating, singleton
	music = new Music();

	return 0;
}

// Main game loop.
int gameLoop()
{
	vector<DriveControl*> playerControls; 
	DriveControl* player1 = new DriveControl;
	playerControls.push_back(player1);
	
	while (true)
	{
		control->update();	
		playerControls[0]->steer = control->getLeftThumbX(0) / -32768.0f;
		playerControls[0]->accel = control->getRightTrigger(0) / 255.0f > control->getLeftTrigger(0) / 255.0f ? control->getRightTrigger(0) / 255.0f : control->getLeftTrigger(0) / -255.0f;
		playerControls[0]->braking = control->getBDown(0) ? 1.0f : 0.0f;
		physicsManager->Update(&playerControls, aiManager->getAIControls());	// Do physics updates
		renderer->Update(entityManager);   // Draw stuff
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities
		aiManager->Update();		// Update AI
		music->update();

		//reload game constants from config on key 'o' (oh)
		if (keyboard->getKeyPressed(79) == true){
			static SettingsFile file(CONFIG_FILE);
			if (file.reloadFile() == true){
				printf("Config file reloaded\n");
				printf("%i\n", file.getInt("bot-count"));

				//SettingsFile::printWorkingDir();
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

	return 0;
}