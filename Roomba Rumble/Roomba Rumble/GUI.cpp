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

	GLfloat xWid = getWordWidth("time", 15.0f);
	timeTrans = vec3(wWidth/2.0f - xWid, 50.0f, 0.0f);

	damWordTrans = vec3(10.0f, wHeight - wordHeight, 0.0f);

	xWid = getWordWidth("Kills:", 15.0f) + getWordWidth("vv", 15.0f);
	killWordTrans = vec3(wWidth - xWid, 50.0f, 0.0f);

	xWid = getWordWidth("a", 15.0f) + getWordWidth("a", 15.0f);
	killsTrans = vec3(wWidth - xWid, 50.0f, 0.0f);

	xWid = getWordWidth("Damage:", 15.0f) + getWordWidth(":", 15.0f);
	damageTrans = vec3(10.0f + xWid, wHeight - wordHeight, 0.0f);

	respawning = false;
}

GUI::~GUI()
{
}

GLfloat GUI::getWordWidth(string word, GLfloat scale)
{
	GLuint numSpaces = word.length() - 1;
	GLfloat width = 0.0f;

	if(word == "time")
	{
		width = numbers[0]->width * 4;
		width += letters.at(':')->width + (spaceWidth * 4);
		width *= scale;
		width /= 2.0f;
	}
	else
	{
		for(GLuint i = 0; i < word.length(); i++)
		{
			width += letters.at(word[i])->width;
		}
		width += spaceWidth * numSpaces;
		width *= scale;
	}


	return width;
}

void GUI::loadObjects()
{
	for(GLuint i = 0; i < 10; i++)
	{
		numbers[i] = new GraphicsObject(rManager->numbers[i]);
		numbers[i]->normals = refactorNormals(numbers[i]->normals);
		numbers[i]->bindBuffer(false);
		numbers[i]->genBuffer();
	}

	for(GLuint i = 0; i < NUM_LETTERS; i++)
	{
		char index = (char)(i+ALPHABET_OFFSET);
		map<char, obj*>::iterator it = rManager->letters.find(index);
		if(it != rManager->letters.end())
		{
			letters.insert(pair<char, GraphicsObject*>(index, new GraphicsObject(rManager->letters.at(index))));
			letters.at(index)->normals = refactorNormals(letters.at(index)->normals);
			letters.at(index)->bindBuffer(false);
			letters.at(index)->genBuffer();
		}
	}
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

vector<GraphicsObject*> GUI::fetchWord(string word)
{
	vector<GraphicsObject*> gBuffer;

	for(GLuint i = 0; i < word.length(); i++)
	{
		if(word[i] != ' ')
			gBuffer.push_back(letters.at(word[i]));
		else
			gBuffer.push_back(NULL);
	}
	
	return gBuffer;
}

void GUI::drawWord(string key, vec3 ambient, vec3 translate, GLfloat scalarX, GLfloat scalarY)
{
	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));

	vec3 scaleVec = vec3(scalarX, scalarY, 0.0f);
	GLfloat inc = 0.0f;
	vector<GraphicsObject*> gBuffer = fetchWord(key);
		
	for(GLuint i = 0; i < gBuffer.size(); i++)
	{
		if(gBuffer[i] == NULL)
			inc += 1.0f * scalarX;
		else
		{
			vec3 trans = translate;
			trans.x += inc;
			gBuffer[i]->draw(ambient, trans, scaleVec, modelView, shaderIDs);
			inc += ((gBuffer[i]->width+spaceWidth) * scalarX);
		}
	}

}

void GUI::drawTime(GLint gameTime, vec3 ambient, vec3 scaleVec)
{
	myTime time = getTime(gameTime);
	vec3 translateVec = timeTrans;
	GLfloat inc = 0;

	numbers[time.minuteLeft]->draw(ambient, translateVec, scaleVec, modelView, shaderIDs);
	inc += ((numbers[time.minuteLeft]->width+spaceWidth) * scaleVec.x);
	translateVec.x = timeTrans.x + inc;

	numbers[time.minuteRight]->draw(ambient, translateVec, scaleVec, modelView, shaderIDs);
	inc += ((numbers[time.minuteRight]->width+spaceWidth) * scaleVec.x);
	translateVec.x = timeTrans.x + inc;

	letters.at(':')->draw(ambient, translateVec, scaleVec, modelView, shaderIDs);
	inc += ((letters.at(':')->width+spaceWidth) * scaleVec.x);
	translateVec.x = timeTrans.x + inc;

	numbers[time.secondLeft]->draw(ambient, translateVec, scaleVec, modelView, shaderIDs);
	inc += ((numbers[time.secondLeft]->width+spaceWidth) * scaleVec.x);
	translateVec.x = timeTrans.x + inc;

	numbers[time.secondRight]->draw(ambient, translateVec, scaleVec, modelView, shaderIDs);
}

void GUI::drawStaticElements(GLint gameOver)
{
	if(gameOver == 1)
		drawWord("You Win", WHITE, winMessageTrans, 40.0f, 50.0f);
	else if(gameOver == 2)
		drawWord("You Lose", WHITE, loseMessageTrans, 40.0f, 50.0f);
	else if(respawning == true)
		drawWord("Respawning", WHITE, respawnMessageTrans, 40.0f, 50.0f);
	else
	{
		drawWord("Kills:", RED, killWordTrans, 15.0f, 15.0f);
		drawWord("Damage:", BLUE, damWordTrans, 15.0f, 15.0f);
	}
}

void GUI::showRespawning(){
	respawning = true;
}

void GUI::hideRespawning(){
	respawning = false;
}

void GUI::drawDynamicElements(GLint gameTime, GLint damage, GLint kills, GLfloat health)
{
	vec3 ambient;
	vec3 scaleVec = vec3(15.0f, 15.0f, 0.0f);
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
	else
	{
		ambient = vec3(1.0f, 1.0f, 1.0f);
		scaleVec = vec3(15.0f, 15.0f, 0.0f);
		drawTime(0, ambient, scaleVec);
	}
	drawHealth(health);
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