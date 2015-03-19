#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "PowerupObject.h"
#include "Material.h"
#include <vector>

enum shaderLocationIDs{
	ambient = 0,
	diffuse,
	specAlb,
	specPow,
	texObj,
	mvMat,
	projMat,
	lightPos
};

class GraphicsObject{
private:
	GLuint activePowerup;
	const char* tag;
	bool alive;
	GLuint numIndices;

public:
	PowerupObject melee, defense, ranged;

	std::vector<GLfloat> vertices, normals, texVertices;
	std::vector<GLuint> indices, normIndices, texIndices;
	glm::quat rotationQuat;
	glm::vec3 translateVector;
	Material material;
	GLubyte* tgaBits;
	std::string textureFile;
	GLuint VAO, TBO, VBO;
	glm::vec3 max, min, center;

	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	GLuint getNumIndices();
	void setNumIndices();
	GLuint bufferSize();
	void findMax();
	void findMin();
	void findCenter();

	void bindBuffer();
	void genBuffer();
	GLboolean loadTexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode);
	GLboolean readTGABits(GLuint &tWidth, GLuint &tHeight, GLuint &tComponents, GLenum &eFormat);
	void draw(glm::mat4 modelView, GLuint *shaderIDs);

	GLuint getActivePow();
	void setActivePow(GLuint pow);
	void setTag(const char* newTag){ tag = newTag;}
	const char* getTag(){ return tag;}
	void setAlive(bool flag){ alive = flag; }
	void destroy(){ alive = false; }
	bool isAlive(){ return alive; }


	void clear();
	void clearPowData();
	void rearrangeData();
};
