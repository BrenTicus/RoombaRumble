#include "GUI.h"

#define VERTEX_DATA 0

GLfloat vertices [] = {
	10.0f, 10.0f, 1.0f, 1.0f,
	10.0f, 10.0f, 1.0f, 1.0f,
	10.0f, 10.0f, 1.0f, 1.0f,
		
	10.0f, 10.0f, 1.0f, 1.0f,
	10.0f, 10.0f, 1.0f, 1.0f,
	10.0f, 10.0f, 1.0f, 1.0f
};

GUI::GUI(GLuint width, GLuint height, GLuint* shaders)
{
	wWidth = (GLfloat)width;
	wHeight = (GLfloat)height;
	projection = glm::ortho(0.0f, wWidth, wHeight, 0.0f);
	modelView = mat4(1.0f);
	shaderIDs = shaders;
	maxHP = 11.0f;
	rManager = ResourceManager::mainResourceManager;
	wordKills = new GraphicsObject(rManager->wordKills);

	wordKills->normals = refactorNormals(wordKills->normals);
	wordKills->vertices = myTranslate(wordKills->vertices, 40.0f, 0.0f);
	wordKills->bindBuffer(false);
	wordKills->genBuffer();
}

GUI::~GUI()
{
}

void GUI::bindBuffer(GLuint &VAO, GLuint &VBO)
{

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData (GL_ARRAY_BUFFER, 0, 24 * sizeof(GLfloat), vertices);

	glEnableVertexAttribArray(VERTEX_DATA);
	glVertexAttribPointer(VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	
}

vector<GLfloat> GUI::myTranslate(vector<GLfloat> verts, GLfloat deltaX, GLfloat deltaY)
{
	for(GLuint i = 0; i < verts.size(); i += 4)
	{
		verts[i] += deltaX;
		verts[i+1] += deltaY;
	}
	return verts;
}

vector<GLfloat> GUI::refactorNormals(vector<GLfloat> normals)
{
	for(GLuint i = 0; i < normals.size(); i += 3)
	{
		normals[i] = 0.0f;
		normals[i+1] = 0.0f;
		normals[i+2] = 1.0f;
	}
	return normals;
}

void GUI::drawWord(const char* key)
{
	vec3 ambient = vec3(1.0f, 0.1f, 0.1f);
	wordKills->draw(ambient, projection, modelView, shaderIDs);
}


GLboolean GUI::drawHealth(GLfloat health)
{
	GLuint VAO, VBO;
	GLfloat hpHeight = 25.0f;
	GLfloat hpScale = 15.0f;

	if(health == 0)
		return 0;
	else if(health > maxHP)
		health = maxHP;

	vertices[5] = hpHeight;
	vertices[8] = health * hpScale;
	vertices[9] = hpHeight;
	vertices[12] = health * hpScale;
	vertices[13] = hpHeight;
	vertices[16] = health * hpScale;

	bindBuffer(VAO, VBO);

	if(health < 2.0f)
		glUniform3f(shaderIDs[ambient], 1.0f, 0.0f, 0.0f); 
	else if(health < 4.0f)
		glUniform3f(shaderIDs[ambient], 1.0f, 1.0f, 0.0f); 
	else
		glUniform3f(shaderIDs[ambient], 0.0f, 1.0f, 0.1f); 

	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));
	glUniformMatrix4fv(shaderIDs[mvMat], 1, GL_FALSE, glm::value_ptr(modelView));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	return true;
}