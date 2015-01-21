#pragma once

#include <vector>
#include "Entity.h"

class EntityManager
{
public:
	std::vector<Entity> entityList;

	EntityManager();
	~EntityManager();

	int Update();
};

