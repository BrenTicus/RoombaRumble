#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Entity.h"
#include "EntityManager.h"
#include "Material.h"
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
	GLuint numStaticObjects, numDynamicObjects;

	vec3 roombaPosition;
	mat4 modelView, projection;
	vector<GLfloat> vertices, vBuffer;
	vector<GLfloat> normals, nBuffer;
	vector<GLuint> faceIndices;
	vector<GLuint>	normIndices;
	vector<vec3> dTransVectors, sTransVectors;
	vector<quat> dRotateQuats, sRotateQuats;
	vector<GLuint> faceSizes;

	vector<Material> dMaterials, sMaterials;
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
	void drawScene(int width, int height);
	void drawObject(vec3 translate, vec3 scale, quat rotate, Material m, GLint start, GLsizei count);
	void Update(EntityManager* eManager);
};

