#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Entity.h"
#include "EntityManager.h"
#include "Material.h"
#include "GraphicsObject.h"
#include "stdio.h"
#include <iostream>

using namespace glm;

class Renderer
{
private:
	EntityManager* eManager;

	GLFWwindow* window;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;
	GLuint vertexBuffer;
	
	vector<GraphicsObject> gObjList;
	vector<GLfloat> vBuffer, nBuffer;
	vec3 roombaPosition;
	mat4 modelView, projection;
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
	void genBuffers();
	void drawScene(int width, int height);
	void drawObject(GraphicsObject gObj, vec3 scale, GLint start, GLsizei count);
	void Update(EntityManager* eManager);
};

