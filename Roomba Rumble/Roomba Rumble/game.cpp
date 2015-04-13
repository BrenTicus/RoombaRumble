#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Sound.h"
#include "Keyboard.h"
#include "SettingsFile.h"
#include <iostream>

Controller* control;
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

	control = entityManager->getController();

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

/*void PressA()
{

}

void PressB()
{

}

void ShowInstructions(int x)
{
	//Display Instructions Image
	//renderer->menu(2);
	system("pause");
	if (x==0)
		Game::_gameState = Game::Menu;
	else if (x==1)
		Game::_gameState = Game::Paused;

}

void ShowMenu()
{
	pos = 0;
	//set highlight bar under Play & pos = 0
	while (Game::_gameState == Game::Menu) 
	{ 
		renderer->menu(0, pos);
		control->update();
		//Moving the highlight using thumbstick or keyboard
		if ((control->getLeftThumbY(0) > 0) || (keyboard->getAccel_Pressed() == true))
		{
			if (pos != 0)
				pos--;
		}
		if ((control->getLeftThumbY(0) < 0) || (keyboard->getBraking_Pressed() == true))
		{
			if (pos != 2)
				pos++;
		}

		//Move Bar accordingly to current pos
		//renderer->Update();

		//Selecting a Menu Option
		if((control->getADown(0) == true) && (pos == 0))
		{
			Game::_gameState = Game::Playing;
		}
		if((control->getADown(0) == true) && (pos == 1))
		{
			ShowInstructions(0);
		}
		if((control->getADown(0) == true) && (pos == 2))
		{
			Game::_gameState = Game::Exiting;
		}
		if((control->getBDown(0) == true) && (pos == 2))
		{
			Game::_gameState = Game::Exiting;
		}
	}
}

void ShowPause()
{
	pos = 0;
	//set highlight bar under Play & pos = 0
	while (Game::_gameState == Game::Paused) 
	{ 
		renderer->menu(1, pos);
		//Moving the highlight using thumbstick or keyboard
		if ((control->getLeftThumbY(0) > 0) || (keyboard->getAccel_Pressed() == true))
		{
			if (pos != 0)
				pos--;
		}
		if ((control->getLeftThumbY(0) < 0) || (keyboard->getBraking_Pressed() == true))
		{
			if (pos != 3)
				pos++;
		}


		//Selecting a Menu Option
		if((control->getADown(0) == true) && (pos == 0))
		{
			Game::_gameState = Game::Playing;
		}
		if((control->getADown(0) == true) && (pos == 1))
		{
			ShowInstructions(1);
		}
		if((control->getADown(0) == true) && (pos == 2))
		{
			Game::_gameState = Game::Menu;
		}
		if((control->getADown(0) == true) && (pos == 3))
		{
			Game::_gameState = Game::Exiting;
		}
		if((control->getBDown(0) == true) && (pos == 0))
		{
			Game::_gameState = Game::Playing;
		}
	}
}

void ShowFinal()
{
	pos = 0;
	//set highlight bar under Play & pos = 0
	while (Game::_gameState == Game::Final) 
	{ 
		renderer->menu(3, pos);
		//Moving the highlight using thumbstick or keyboard
		if ((control->getLeftThumbY(0) > 0) || (keyboard->getAccel_Pressed() == true))
		{
			if (pos != 0)
				pos--;
		}
		if ((control->getLeftThumbY(0) < 0) || (keyboard->getBraking_Pressed() == true))
		{
			if (pos != 2)
				pos++;
		}

		//Move Bar accordingly to current pos
		//renderer->Update();

		//Selecting a Menu Option
		if((control->getADown(0) == true) && (pos == 0))
		{
			Game::_gameState = Game::Playing;
		}
		if((control->getADown(0) == true) && (pos == 1))
		{
			Game::_gameState = Game::Menu;
		}
		if((control->getADown(0) == true) && (pos == 2))
		{
			Game::_gameState = Game::Exiting;
		}

	}
}*/

// Main game loop.
int gameLoop()
{
	control->registerButtonEvent(0x1000, 0, PressA);
	control->registerButtonEvent(0x2000, 0, PressB);

/*	while(Game::_gameState!=Game::Exiting)
	{
		switch(Game::_gameState)
		{
		case Game::Menu : 
		{
			ShowMenu();
			break;
		}
		case Game::Paused :
		{
			ShowPause();
			break;
		}
		case Game::Final :
		{	
			ShowFinal();
			break;
		}
		case Game::Playing :
		{*/
			float lastRespawn = (float)clock();
			float lastAIUpdate = (float)clock();
			float lastAIRespawn = (float)clock();
		
			Camera* cam = renderer->getCamera();
		
			
		
			while (true)
			{
				physicsManager->Update();	// Do physics updates
				renderer->Update();   // Draw stuff
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
				}
				else if(!winnerFlag && renderer->getGameOver() == 2)
				{
					//lose
					sound->playSound("you_lose.wav");
					winnerFlag = true;
				}
		
				
			}
		//	break;
	//	}
//	}
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
