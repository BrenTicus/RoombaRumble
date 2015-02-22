#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Controller.h"
#include "Music.h"
#include "Keyboard.h"

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;
Keyboard* keyboard;
Controller* control;
Music* music;


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
		entityManager->Update();	// Update entities
		renderer->Update(entityManager);   // Draw stuff
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		music->update();
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