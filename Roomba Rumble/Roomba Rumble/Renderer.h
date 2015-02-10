#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Entity.h"
#include "EntityManager.h"
#include "stdio.h"
#include <iostream>

using namespace glm;

class Renderer
{
private:
	GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;
	GLuint VAO;
	GLuint vertexBuffer, indexBuffer;

	vec3 roombaPosition;
	mat4 modelView, projection;
	vector<GLfloat> vertices, vBuffer;
	vector<GLfloat> normals, nBuffer;
	vector<GLuint> faceIndices;
	vector<GLuint>	normIndices;
	vector<vec3> scaleVectors, transVectors, colorVectors;
	vector<quat> rotateQuats;
	vector<int> faceSizes;

	EntityManager* eManager;
public:
	Renderer(EntityManager* eManager);
	~Renderer();
		
	vec4 getVertex (int i);
	vec3 getNormal (int i);

	bool readShader(const char* filename, int shaderType);
	int setupShaders();
	void setupObjectsInScene();
	void updatePositions();
	vector<GLfloat> rearrangeVerts(vector<GLuint> indices);
	vector<GLfloat> rearrangeNorms(vector<GLuint> indices);
	void bindBuffers();
	void drawScene();
	void drawObject(vec3 translate, vec3 scale, quat rotate, vec3 color, GLint start, GLsizei count);
	void Update(EntityManager* eManager);
};

