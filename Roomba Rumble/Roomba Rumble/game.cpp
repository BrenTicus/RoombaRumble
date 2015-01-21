#include "PhysicsManager.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "Renderer.h"

Renderer* renderer;
InputManager* inputManager;
PhysicsManager* physicsManager;
EntityManager* entityManager;

int initialize()
{
	renderer = new Renderer();
	inputManager = new InputManager();
	physicsManager = new PhysicsManager();
	entityManager = new EntityManager();

	return 0;
}

int gameLoop()
{
	while (true)
	{
		physicsManager->Update();
		renderer->Update();
		entityManager->Update();
		inputManager->Update();
	}

	return 0;
}

//Program entry point.
int main()
{
	initialize();
	gameLoop();

	return 0;
}