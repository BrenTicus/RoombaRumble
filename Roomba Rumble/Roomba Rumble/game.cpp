#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;

// Set up major systems.
int initialize()
{
	renderer = new Renderer();
	inputManager = new InputManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager(physicsManager);

	return 0;
}

// Main game loop.
int gameLoop()
{
	while (true)
	{
		physicsManager->Update();	// Do physics updates
		renderer->Update();			// Draw stuff
		physicsManager->LateUpdate();	// Write physics updates so they're usable by everything
		entityManager->Update();	// Update entities
		inputManager->Update();		// Take input
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