#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Sound.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;
Keyboard* keyboard;
Sound* sound;


const std::string CONFIG_FILE = "game_config";									//for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble

// Set up major systems.
int initialize()
{
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
	bool won = false;
	while (true)
	{
		physicsManager->Update();	// Do physics updates
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
		if (entityManager->aiRoombas.size() == 0 && !won)
		{
			//win things
			cout << "victory, yay" << endl;
			won = true;
		}
		if (entityManager->roombas.size() == 0 && !won)
		{
			//lose things
			cout << "ya suck, kid" << endl;
			won = true;
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