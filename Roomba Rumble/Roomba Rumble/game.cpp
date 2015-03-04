#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Controller.h"
#include "Sound.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;
Keyboard* keyboard;
Controller* control;
Sound* sound;


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
	sound = new Sound();
	physicsManager -> sound = sound;
	sound ->playMusic("Kick_their_ass.ogg");

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
		DriveControl* ai = entityManager->getAIControls()->at(0);
		controls[1]->steer = ai->steer;
		controls[1]->accel = ai->accel;
		controls[1]->braking = ai->braking;
		physicsManager->Update(controls);	// Do physics updates
		renderer->Update(entityManager);   // Draw stuff
		entityManager->LateUpdate();	// Clean up entities from last iteration.
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities
		entityManager->UpdateAI();
		sound->update();

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