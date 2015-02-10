#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Controller.h"

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;
Controller* control;
void nothing(){
	printf("Y Button\n");
}
// Set up major systems.
int initialize()
{
	control = new Controller();
	inputManager = new InputManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager(physicsManager);
	renderer = new Renderer(entityManager);
	

	control->registerButtonEvent(0x8000, 0, nothing);
	return 0;
}

// Main game loop.
int gameLoop()
{
	while (true)
	{
		control->update();	
		physicsManager->Update(control->getLeftThumbX(0)/32768.0f, control->getRightTrigger(0)/255.0f);	// Do physics updates
		entityManager->Update();	// Update entities
		renderer->Update(entityManager);   // Draw stuff
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		inputManager->Update();		// Take input
		control->update();			//activate controller events
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