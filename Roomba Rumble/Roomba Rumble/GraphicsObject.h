#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "PowerupObject.h"
#include "Material.h"
#include <vector>

class GraphicsObject{
private:
	GLuint activePowerup;
	const char* tag;
	bool alive;

public:
	PowerupObject melee, defense, ranged;

	std::vector<GLfloat> vertices, normals, texVertices;
	std::vector<GLuint> indices, normIndices, texIndices;
	glm::quat rotationQuat;
	glm::vec3 translateVector;
	Material material;
	GLubyte* tgaBits;
	GLuint tWidth, tHeight, tComponents;
	GLenum eFormat;
	std::string textureFile;
	GLuint VAO, TBO, VBO;
	glm::vec3 max, min, center;

	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	GLuint getNumIndices();
	GLuint bufferSize();
	void findMax();
	void findMin();
	void findCenter();

	GLuint getActivePow();
	void setActivePow(GLuint pow);
	void setTag(const char* newTag){ tag = newTag;}
	const char* getTag(){ return tag;}
	void setAlive(bool flag){ alive = flag; }
	bool isAlive(){ return alive; }

	void clear();
	void rearrangeData();
	GLboolean readTGABits();
};
