#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Material.h"
#include <vector>

class GraphicsObject{

public:
	std::vector<GLfloat> vertices, normals, texVertices;
	std::vector<GLuint> indices, normIndices, texIndices;
	glm::quat rotationQuat;
	glm::vec3 translateVector;
	Material material;
	GLuint VAO;
	GLboolean isDynamic;

	void clear();
	void rearrangeData();
	GLfloat* getData(int type);
	GLuint getSize(int type);
	GLuint getNumIndices();
	GLuint bufferSize();
};