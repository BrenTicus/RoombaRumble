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
	loadObjects();
}

GUI::~GUI()
{
}

void GUI::loadObjects()
{
	wordKills = new GraphicsObject(rManager->wordKills);
	wordKills->normals = refactorNormals(wordKills->normals);
	wordKills->bindBuffer(false);
	wordKills->genBuffer();
	
	wordDamage = new GraphicsObject(rManager->wordDamage);
	wordDamage->normals = refactorNormals(wordDamage->normals);
	wordDamage->bindBuffer(false);
	wordDamage->genBuffer();

	for(GLuint i = 0; i < 10; i++)
	{
		numbers[i] = new GraphicsObject(rManager->numbers[i]);
		numbers[i]->normals = refactorNormals(numbers[i]->normals);
		numbers[i]->bindBuffer(false);
		numbers[i]->genBuffer();
	}

	symbolColon = new GraphicsObject(rManager->symbolColon);
	symbolColon->normals = refactorNormals(symbolColon->normals);
	symbolColon->bindBuffer(false);
	symbolColon->genBuffer();
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

myTime GUI::getTime(GLuint bulk)
{
	myTime time;
	GLint minutes = bulk / 60;
	GLint seconds = bulk % 60;

	if(minutes < 10)
	{
		time.minuteLeft = 0;
		time.minuteRight = minutes;
	}
	else
	{
		time.minuteLeft = minutes / 10;
		time.minuteRight = minutes % 10;
	}

	if(seconds < 10)
	{
		time.secondLeft = 0;
		time.secondRight = seconds;
	}
	else
	{
		time.secondLeft = seconds / 10;
		time.secondRight = seconds % 10;
	}

	return time;
}

void GUI::drawWord(const char* key)
{
	vec3 ambient, scaleVec;

	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));

	if(strcmp(key, "kills") == 0)
	{
		ambient = vec3(1.0f, 0.0f, 0.0f);
		scaleVec = vec3(20.0f, 20.0f, 0.0f);

		wordKills->draw(ambient, killWordTrans, scaleVec, modelView, shaderIDs);
	}
	else if(strcmp(key, "damage") == 0)
	{
		ambient = vec3(0.0f, 0.0f, 1.0f);
		scaleVec = vec3(20.0f, 20.0f, 0.0f);

		wordDamage->draw(ambient, damWordTrans, scaleVec, modelView, shaderIDs);
	}
}

void GUI::drawTime(GLint gameTime, vec3 ambient, vec3 scaleVec)
{
	vec3 translateVec;
	myTime time = getTime(gameTime);

	numbers[time.minuteLeft]->draw(ambient, minLeftTrans, scaleVec, modelView, shaderIDs);
	numbers[time.minuteRight]->draw(ambient, minRightTrans, scaleVec, modelView, shaderIDs);
	symbolColon->draw(ambient, colonTrans, scaleVec, modelView, shaderIDs);
	numbers[time.secondLeft]->draw(ambient, secLeftTrans, scaleVec, modelView, shaderIDs);
	numbers[time.secondRight]->draw(ambient, secRightTrans, scaleVec, modelView, shaderIDs);
}

void GUI::drawStaticElements()
{
	drawWord("kills");
	drawWord("damage");
}

void GUI::drawDynamicElements(GLint gameTime, GLint damage, GLint kills)
{
	vec3 ambient;
	vec3 scaleVec = vec3(20.0f, 20.0f, 0.0f);
	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));
	
	if(damage >= 10 && damage < 150)
		damage = 1;

	if(damage >= 0 && damage < 10)
	{
		ambient = vec3(0.0f, 0.0f, 1.0f);
		numbers[damage]->draw(ambient, damageTrans, scaleVec, modelView, shaderIDs);
	}
		
	if(kills >= 0 && kills < KILLS_TO_WIN)
	{
		ambient = vec3(1.0f, 0.0f, 0.0f);
		numbers[kills]->draw(ambient, killsTrans, scaleVec, modelView, shaderIDs);
	}

	if(gameTime > 0)
	{
		ambient = vec3(1.0f, 1.0f, 1.0f);
		scaleVec = vec3(15.0f, 15.0f, 0.0f);
		drawTime(gameTime, ambient, scaleVec);
	}
}

GLboolean GUI::drawHealth(GLfloat health)
{
	GLuint VAO, VBO;
	GLfloat hpHeight = 25.0f;
	GLfloat hpScale = 15.0f;

	if(health == 0)
		return false;
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