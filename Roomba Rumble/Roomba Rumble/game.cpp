#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Controller.h"
#include "Music.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;
Keyboard* keyboard;
Controller* control;
Music* music;


const std::string CONFIG_FILE = "game_config";									//for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble

// Set up major systems.
int initialize()
{
	control = new Controller();
	inputManager = new InputManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager(physicsManager);
	renderer = new Renderer(entityManager);
	keyboard = Keyboard::getInstance(renderer->getWindow());			//keyboard does not need updating, singleton
	music = new Music();

	return 0;
}

// Main game loop.
int gameLoop()
{
	DriveControl* controls[2]; 
	controls[0] = new DriveControl;
	controls[1] = new DriveControl;
	while (true)
	{
		control->update();	
		controls[0]->steer = control->getLeftThumbX(0) / -32768.0f;
		controls[0]->accel = control->getRightTrigger(0) / 255.0f > control->getLeftTrigger(0) / 255.0f ? control->getRightTrigger(0) / 255.0f : control->getLeftTrigger(0) / -255.0f;
		controls[0]->braking = control->getBDown(0) ? 1.0f : 0.0f;
		controls[1]->steer = 0.75f;
		controls[1]->accel = 1.0f;
		controls[1]->braking = 0.0f;
		physicsManager->Update(controls);	// Do physics updates
		renderer->Update(entityManager);   // Draw stuff
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities
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