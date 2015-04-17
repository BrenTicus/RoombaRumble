#include "GUI.h"

#define VERTEX_DATA 0

GLfloat vertices [] = {
	15.0f, 10.0f, 1.0f, 1.0f,
	15.0f, 10.0f, 1.0f, 1.0f,
	15.0f, 10.0f, 1.0f, 1.0f,
		
	15.0f, 10.0f, 1.0f, 1.0f,
	15.0f, 10.0f, 1.0f, 1.0f,
	15.0f, 10.0f, 1.0f, 1.0f
};

GLfloat normals [] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f
};

GLfloat texCoords [] = {
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
};

GLuint indices [] = {
	0, 1, 3,
	0, 2, 3
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
	//powTrans = vec3(10.0f, wHeight - wordHeight, 0.0f);

	xWid = getWordWidth("Kills:", 15.0f) + getWordWidth("vvv", 15.0f);
	killWordTrans = vec3(wWidth - xWid, 50.0f, 0.0f);

	xWid = getWordWidth("vv", 15.0f) + getWordWidth("n", 15.0f);
	killsTrans = vec3(wWidth - xWid, 50.0f, 0.0f);

	xWid = getWordWidth("Weapon Lvl:", 15.0f) + getWordWidth(":", 15.0f);
	weaponLvlTrans = vec3(15.0f + xWid, wHeight - wordHeight, 0.0f);

	xWid = getWordWidth("Respawning", 40.0f);
	xWid  += (wWidth - xWid) / 2.0f;
	respawnMessageTrans = vec3(wWidth - xWid, wHeight / 2.0f, 0.0f);
	
	xWid = getWordWidth("You Win", 30.0f);
	xWid  += (wWidth - xWid) / 2.0f;
	winMessageTrans = vec3(wWidth - xWid, wHeight / 2.4f, 0.0f);
	
	xWid = getWordWidth("You Lose", 30.0f);
	xWid  += (wWidth - xWid) / 2.0f;
	loseMessageTrans = vec3(wWidth - xWid, wHeight / 2.4f, 0.0f);

	menuBacking = getMenuBacking(wWidth, wHeight);

	vector<GLuint> ind(begin(indices), end(indices));
	vector<GLfloat> norms(begin(normals), end(normals));
	vector<GLfloat> tex(begin(texCoords), end(texCoords));
	backing = new obj();

	backing->vertices = &menuBacking;
	backing->normals = &norms;
	backing->texVertices = &tex;
	backing->faceIndices = &ind;
	backing->normIndices = &ind;
	backing->texIndices = &ind;
	
	menu[0] = new GraphicsObject(backing, "Assets/GUI/Menu/mainmenu1600x900.tga");
	menu[1] = new GraphicsObject(backing, "Assets/GUI/Menu/pausemenu1600x900.tga");
	menu[2] = new GraphicsObject(backing, "Assets/GUI/Menu/gameovermenu1600x900.tga");
	menu[3] = new GraphicsObject(backing, "Assets/GUI/Menu/instructions1600x900.tga");
	
	underline = new GraphicsObject(rManager->underline);

	menuBacking = getIconBacking(wWidth, wHeight);
	backing->vertices = &menuBacking;
	powerupIcon = new GraphicsObject(backing, "Assets/GUI/icons.tga");

	box = new GraphicsObject(rManager->box);

	respawning = false;
}

GUI::~GUI()
{
	for (unsigned int i = 0; i < 3; i++)
	{
		delete menu[i];
	}
	delete underline;
	delete box;
	delete backing;
	delete powerupIcon;
	for (unsigned int i = 0; i < 10; i++)
	{
		delete numbers[i];
	}
	for (unsigned int i = 0; i < NUM_LETTERS + 1; i++)
	{
		delete letters[i];
	}
}
vector<GLfloat> GUI::alterTCoords(vector<GLfloat> tCoords, GLuint whichThird)
{
	vector<GLfloat> coords = tCoords;

	if(whichThird == 1)
	{

	}
	else if(whichThird == 2)
	{
	}
	else if(whichThird == 3)
	{
	}

	return coords;
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
			if(word[i] == ' ')
				width += 1.0f;
			else
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
	}

	for(GLuint i = 0; i < NUM_LETTERS; i++)
	{
		char index = (char)(i+ALPHABET_OFFSET);
		map<char, obj*>::iterator it = rManager->letters.find(index);
		if(it != rManager->letters.end())
		{
			letters.insert(pair<char, GraphicsObject*>(index, new GraphicsObject(rManager->letters.at(index))));
		}
	}

	letters.insert(pair<char, GraphicsObject*>('.', new GraphicsObject(rManager->letters.at('.'))));
}

vector<GLfloat> GUI::getMenuBacking(GLfloat width, GLfloat height)
{
	vector<GLfloat> verts;
	
	verts.push_back(0.0f);
	verts.push_back(0.0f);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(width);
	verts.push_back(0.0f);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(0.0f);
	verts.push_back(height);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(width);
	verts.push_back(height);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	return verts;
}
vector<GLfloat> GUI::getIconBacking(GLfloat width, GLfloat height)
{
	vector<GLfloat> verts;
	
	verts.push_back(0.0f);
	verts.push_back(height - 200.0f);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(300.0f);
	verts.push_back(height - 200.0f);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(0.0f);
	verts.push_back(height);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	verts.push_back(300.0f);
	verts.push_back(height);
	verts.push_back(0.0f);
	verts.push_back(1.0f);

	return verts;
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

void GUI::drawMenu(GLuint menuIndex, int pos)
{
	if(menuIndex >= NUM_MENUS)
		return;
	
	vec3 scaleVec, lineTranslate;
	if (menuIndex == 0)
	{

		//
		scaleVec = vec3(30.0f, 52.0f, 0.0f);
		if (pos == 0)
			lineTranslate = vec3(wWidth/2.3, wHeight/1.55f, 0.0f);
		else if (pos == 1)
			lineTranslate = vec3(wWidth/2.3, wHeight/1.35f, 0.0f);
		else
			lineTranslate = vec3(wWidth/2.3, wHeight/1.19f, 0.0f);
	}
	else if (menuIndex == 1)
	{

		//pause menu
		scaleVec = vec3(43.5f, 52.0f, 0.0f);
		if (pos == 0)
			lineTranslate = vec3(wWidth/2.52, wHeight/2.33f, 0.0f);
		else if (pos == 1)
			lineTranslate = vec3(wWidth/2.52, wHeight/1.9f, 0.0f);
		else if (pos == 2)
			lineTranslate = vec3(wWidth/2.52, wHeight/1.6f, 0.0f);
		else
			lineTranslate = vec3(wWidth/2.52, wHeight/1.39f, 0.0f);
	}
	else if (menuIndex == 2)
	{
		//final screen
		scaleVec = vec3(36.5f, 52.0f, 0.0f);
		if (pos == 0)
			lineTranslate = vec3(wWidth/2.405f, wHeight/2.0f, 0.0f);
		else if (pos == 1)
			lineTranslate = vec3(wWidth/2.405f, wHeight/1.68f, 0.0f);
		else
			lineTranslate = vec3(wWidth/2.405f, wHeight/1.45f, 0.0f);
	}
	else
	{
		scaleVec = vec3(0.0f, 0.0f, 0.0f);
		lineTranslate = vec3(0.0f, 0.0f, 0.0f);
		
	}
	
	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));
	menu[menuIndex]->drawMenu(wWidth, wHeight, modelView, shaderIDs);
	underline->draw(WHITE, lineTranslate, scaleVec, modelView, shaderIDs);
}

void GUI::drawStaticElements(GLint gameOver)
{
	if(gameOver == 1)
		drawWord("You Win", WHITE, winMessageTrans, 40.0f, 50.0f);
	else if(gameOver == 2)
		drawWord("You Lose", WHITE, loseMessageTrans, 40.0f, 50.0f);
	else if(respawning && !(gameOver == 1) && !(gameOver == 2))
		drawWord("Respawning", WHITE, respawnMessageTrans, 40.0f, 50.0f);
	else
	{
		vec3 scaleVec = vec3(1.0f, 1.0f, 0.0f);
		drawWord("Kills:", RED, killWordTrans, 15.0f, 15.0f);
		powerupIcon->drawMenu(wWidth, wHeight, modelView, shaderIDs);
	}
}


void GUI::drawDynamicElements(GLint gameTime, GLint pLevel, GLint pType, GLint kills, GLfloat health, vector<scoreID> scoreBoard, GLint gameOver)
{
	vec3 ambient;
	vec3 scaleVec = vec3(15.0f, 15.0f, 0.0f);
	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, glm::value_ptr (projection));
	
	if(pLevel >= 10 && pLevel < 150)
		pLevel = 1;

	if(pLevel >= 0 && pLevel < 10)
	{
		ambient = BLUE;
		//if(!respawning) numbers[pLevel]->draw(ambient, weaponLvlTrans, scaleVec, modelView, shaderIDs);
	}
		
	if(kills >= 0 && kills < KILLS_TO_WIN)
	{
		if(!respawning)
		{
			ambient = RED;
			if(kills < 10)
			{
				numbers[kills]->draw(ambient, killsTrans, scaleVec, modelView, shaderIDs);
			}
			else
			{
				GLint left = kills / 10;
				GLint right = kills % 10;

				numbers[left]->draw(ambient, killsTrans, scaleVec, modelView, shaderIDs);
				vec3 newTrans = killsTrans;
				newTrans.x += (numbers[left]->width * scaleVec.x) + (spaceWidth * scaleVec.x);
				numbers[right]->draw(ambient, newTrans, scaleVec, modelView, shaderIDs);
			}
		}
	}

	if(gameTime > 0)
		drawTime(gameTime, WHITE, scaleVec);
	else
		drawTime(0, WHITE, scaleVec);

	if(!respawning)
	{
		drawHealth(health);
		drawTopScores(scoreBoard, 10.0f, 10.0f, 3);
		drawPowerupIndicator(pLevel, pType);
	}
}

void GUI::drawPowerupIndicator(GLint level, GLint type)
{
	if(type == 0)
		return;

	vec3 translate(0.0f);
	vec3 scaleVec(30.0f, 30.0f, 0.0f);

	if(type == SHIELD)
		translate.y = wHeight - 133.0f;
	else if(type == MELEE)
		translate.y = wHeight - 78.0f;
	else if(type == RANGED)
		translate.y = wHeight - 18.0f;

	if(level == 1)
		translate.x = 172.0f;
	else if(level == 2)
		translate.x = 208.0f;
	else if(level == 3)
		translate.x = 247.0f;

	box->draw(BLACK, translate, scaleVec, modelView, shaderIDs);
}


void GUI::drawTopScores(vector<scoreID> scoreBoard, GLfloat scaleX, GLfloat scaleY, GLuint numScores)
{
	GLfloat playerWidth = getWordWidth("Rock Kickass", scaleX);
	GLfloat width = wWidth - (playerWidth * 1.5f);
	GLfloat height = wHeight / 3.0f;

	vec3 translate = vec3(width, height, 0.0f);
	vec3 scaleVec = vec3(scaleX, scaleY, 0.0f);
	vec3 ambient = WHITE;

	vec3 newTrans = translate;

	scoreBoard = sortScores(scoreBoard);
	for(GLuint i = 0; i < numScores; i++)
	{
		scoreID score = scoreBoard[i];

		if(score.name == "Player One")
			ambient = YELLOW;
		else
			ambient = WHITE;

		numbers[i+1]->draw(ambient, newTrans, scaleVec, modelView, shaderIDs);
		newTrans.x += (numbers[3]->width + spaceWidth) * scaleX;

		letters.at('.')->draw(ambient, newTrans, scaleVec, modelView, shaderIDs);
		newTrans.x += (letters.at('.')->width + spaceWidth) * scaleX;
		newTrans.x += (letters.at('B')->width * scaleX);

		drawWord(score.name, ambient, newTrans, scaleX, scaleY);
		newTrans.x += playerWidth + (spaceWidth * scaleX);
		newTrans.x += (1.0f * scaleX);

		GLint left = score.score / 10;
		GLint right = score.score % 10;

		numbers[left]->draw(ambient, newTrans, scaleVec, modelView, shaderIDs);
		newTrans.x += (numbers[left]->width * scaleVec.x) + (spaceWidth * scaleVec.x);
		
		numbers[right]->draw(ambient, newTrans, scaleVec, modelView, shaderIDs);

		newTrans.x = translate.x;
		newTrans.y += 4.0f * scaleY;
	}
}


vector<scoreID> GUI::sortScores(vector<scoreID> scoreBoard)
{
	scoreID temp;
	int length = scoreBoard.size();

	for(GLuint i = length - 1; i > 0; i--)
	{
		GLuint first = 0;
		for(GLuint j = 1; j <= i; j++)
		{
			if(scoreBoard[j].score < scoreBoard[first].score)
				first = j;
		}
		temp = scoreBoard[first];
		scoreBoard[first] = scoreBoard[i];
		scoreBoard[i] = temp;
	}

	return scoreBoard;
}


GLboolean GUI::drawHealth(GLfloat health)
{
	GLuint VAO, VBO;
	GLfloat hpHeight = 40.0f;
	GLfloat hpScale = 50.0f;

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
	glUniformMatrix4fv(shaderIDs[modMat], 1, GL_FALSE, glm::value_ptr(modelView));
	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	return true;
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
