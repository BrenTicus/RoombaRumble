#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Material.h"
#include <vector>

class GraphicsObject{

public:
	std::vector<GLfloat> vertices, normals, textures;
	std::vector<GLuint> indices, normIndices, texIndices;
	glm::quat rotationQuat;
	glm::vec3 translateVector;
	Material material;
	GLuint VAO;
	GLboolean isDynamic;

	void clear(){
		vertices.resize(0);
		normals.resize(0);
		textures.resize(0);
		indices.resize(0);
		normIndices.resize(0);
		texIndices.resize(0);
	}

	GLfloat* getData(int type)
	{
		if(type == 0)
			return vertices.data();
		else if(type == 1)
			return normals.data();
		else
			return textures.data();
	}

	GLuint vertexSize()
	{
		return sizeof(GLfloat) * vertices.size();
	}

	GLuint normalSize()
	{
		return sizeof(GLfloat) * normals.size();
	}

	GLuint bufferSize()
	{
		return sizeof(GLfloat) * (vertices.size() + normals.size());
	}
};