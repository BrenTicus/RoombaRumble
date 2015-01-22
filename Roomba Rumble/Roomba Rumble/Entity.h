#pragma once

#include <vector>
using namespace std;

class Entity
{
private:
	vector<GLfloat> vertices, normals, textureVertices;
public:
	Entity();
	~Entity();
};

