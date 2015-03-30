#pragma once
#include "Entity.h"
class StaticObject : public Entity
{
public:
	StaticObject();
	StaticObject(vec3 position, string modelName);
	~StaticObject();
};

