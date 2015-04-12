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

const int wWidth = 1600;
const int wHeight = 900;

bool winnerFlag;

const std::string CONFIG_FILE = "game_config"; //for project reference it is in C:\..\GitHub\RoombaRumble\Roomba Rumble\Roomba Rumble
const int GAME_TIME = 180;

// Set up major systems.
int initialize()
{
	winnerFlag = false;
	resourceManager = new ResourceManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager();
	renderer = new Renderer(GAME_TIME, wWidth, wHeight);
	keyboard = Keyboard::getInstance(renderer->getWindow());			//keyboard does not need updating, singleton
	sound = new Sound();
	physicsManager -> sound = sound;
	entityManager -> sound = sound;
	sound ->playMusic("Kick_their_ass.ogg");

	return 0;
}

void shutdown()
{
	delete sound;
	delete renderer;
	delete entityManager;
	delete physicsManager;
	delete resourceManager;
}

// Main game loop.
int gameLoop()
{
	float lastRespawn = (float)clock();
	float lastAIUpdate = (float)clock();
	float lastAIRespawn = (float)clock();
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

		if(clock() - lastAIRespawn > SPAWN_AI_COOLDOWN)
		{
			entityManager->respawnRoombas();
			lastAIRespawn = (float)clock();
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


		if(!winnerFlag && renderer->getGameOver() == 1)
		{
			//win
			sound->playSound("you_win.wav");
			winnerFlag= true;
		}
		else if(!winnerFlag && renderer->getGameOver() == 2)
		{
			//lose
			sound->playSound("you_lose.wav");
			winnerFlag = true;
		}

		
	}

	return 0;
}

// Program entry point.
int main()
{
	initialize();
	gameLoop();
	shutdown();

	return 0;
}