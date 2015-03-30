#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Sound.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>

Renderer* renderer;
PhysicsManager* physicsManager;
ResourceManager* resourceManager;
EntityManager* entityManager;
Keyboard* keyboard;
Sound* sound;

bool winnerFlag;

const std::string CONFIG_FILE = "game_config";									//for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble

// Set up major systems.
int initialize()
{
	winnerFlag = false;
	resourceManager = new ResourceManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager();
	renderer = new Renderer();
	keyboard = Keyboard::getInstance(renderer->getWindow());			//keyboard does not need updating, singleton
	sound = new Sound();
	physicsManager -> sound = sound;
	entityManager -> sound = sound;
	sound ->playMusic("Kick_their_ass.ogg");

	return 0;
}

// Main game loop.
int gameLoop()
{
	float lastRespawn = (float)clock();
	float lastAIUpdate = (float)clock();
	while (true)
	{
		physicsManager->Update();	// Do physics updates
		renderer->Update();   // Draw stuff
		entityManager->LateUpdate();	// Clean up entities from last iteration.
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities
		
		sound->update();
		

		if(clock() - lastAIUpdate > AI_UPDATE_COOLDOWN){
			entityManager->UpdateAI();
			lastAIUpdate = (float)clock();
		}

		if(clock() - lastRespawn > POWERUP_RESPAWN_COOLDOWN)
		{
			entityManager->respawnPowerups();
			lastRespawn = (float)clock();
		}

		


		//reload game constants from config on key 'o' (oh)
		if (keyboard->getKeyPressed(79) == true){
			static SettingsFile file(CONFIG_FILE);
			if (file.reloadFile() == true){
				printf("Config file reloaded\n");
				printf("%i\n", file.getInt("bot-count"));

				//SettingsFile::printWorkingDir();
			}
		}

		//print player location on key 'p'
		if (keyboard->getKeyPressed(80) == true){
			entityManager->printPlayerLocation();
		}


		if (!winnerFlag && entityManager->getPlayerCount() == 0)
		{
			//lose
			sound->playSound("you_lose.wav");
			winnerFlag = true;
		}
		else if (!winnerFlag && entityManager->getAICount() == 0 )
		{
			//win
			sound->playSound("you_win.wav");
			winnerFlag= true;
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