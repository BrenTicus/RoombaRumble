/*
		The renderer receives the vertices, normals, etc. from the entity list
	in the EntityManager. The EntityManager reads the obj files so the renderer doesn't
	have to. 

		The renderer is designed to take the vertices, normals, face indices, and normal indices
	from the entities (and static objects) contained in the entity list of the EntityManager that is 
	passed into the renderer from the main game loop. The renderer then takes the vertices and normals, 
	rearranges them based on the face indices and normals indices, and then sends them down the graphics 
	pipeline to be drawn. Texture incorporation will occur after the second milestone.

		For determining the position of the objects on screen, the idea is to get the position 
	and rotation from the EntityManager for each entity/static object and use those when 
	rendering each object. 
*/

#include "Renderer.h"

#define MAX_SHADER_SIZE 8192
#define VERTEX 0
#define FRAGMENT 1
#define VERTEX_DATA 0
#define NORMAL_DATA 1
#define TEXTURE_DATA 2

static GLubyte shaderText[MAX_SHADER_SIZE];
GLFWwindow* Renderer::window = NULL;
char* vsFilename = "vertPhong.vs.glsl";
char* fsFilename = "fragPhong.fs.glsl";

GLuint shaderIDs[8];

Renderer::Renderer(int gameTime, GLuint width, GLuint height)
{
	wWidth = (GLfloat)width;
	wHeight = (GLfloat)height;
	if (!window)
	{
		if (glfwInit())
		{
			glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

			// Open a window and create its OpenGL context 
			window = glfwCreateWindow(width, height, "Roomba Rumble", NULL, NULL);
			glfwMakeContextCurrent(window); // Initialize GLEW 
		}
		glewExperimental = GL_TRUE;
		glewInit();
	}

	this->eManager = EntityManager::mainEntityManager;
	rManager = ResourceManager::mainResourceManager;
	rif = eManager->getRif();
	rif.getRendererInfo();

	skybox = new Skybox();

	setupShaders();

	gui = new GUI(width, height, shaderIDs);

	setupObjectsInScene();
	//clearObjData();

	this->gameTime = gameTime;
	timestep = (float)clock();
	gameOver = -1;
	justEnded = false;
	scoreBoard.resize(NUM_ROOMBAS);
}

Renderer::~Renderer()
{
	delete gui;
	delete skybox;
	for (unsigned int i = 0; i < gObjList.size(); i++)
	{
		delete gObjList[i];
	}
	delete ball;
	delete airplane;
	delete shuriken;
	for (unsigned int i = 0; i < attachments.size(); i++)
	{
		delete attachments[i];
	}
	delete mainCamera;
	for (unsigned int i = 0; i < staticList.size(); i++)
	{
		delete staticList[i];
	}
}

void Renderer::killWindow()
{
	glfwTerminate();
}


/*
	Method: setupObjectsInScene
	 -This method transfers the vertices, normals, face indices, and normal indices
	 into separate vector buffers
	 -The vertices are rearranged according to the face indices, and the normal vertices
	 are rearranged according to the normal indices
	 -Initial object position vectors and orientation quaternions are set here as well
	 -The number of indices for each object is stored for use in the glDrawArrays method
*/
void Renderer::setupObjectsInScene(){
	vector<Entity*> entities = eManager->entityList;
	vector<StaticObject*> sObjects = eManager->staticList;

	GLuint rifIndex = 0;

	roombaPosition = entities[0]->getPosition();
	for(GLuint i = 0; i < entities.size(); i++)
	{
		GraphicsObject* gObject = new GraphicsObject(entities[i]->getModel(), rif.textureFileNames[rifIndex], rif.materials[rifIndex], entities[i]->getTag());

		gObject->translateVector = entities[i]->getPosition() - gObject->center;//Use center of the object as a reference to find the translation vector
		gObject->rotationQuat = entities[i]->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		gObjList.push_back(gObject);

		if(strcmp(entities[i]->getTag(), "powerup") == 0)
			powerupList.push_back(gObject);

		rifIndex++;
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		GraphicsObject* gObject = new GraphicsObject(sObjects[i]->getModel(), rif.textureFileNames[rifIndex], rif.materials[rifIndex], sObjects[i]->getTag());

		gObject->translateVector = sObjects[i]->getPosition() - gObject->center;//Use center of the object as a reference to find the translation vector
		gObject->rotationQuat = sObjects[i]->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		staticList.push_back(gObject);

		rifIndex++;
	}
	
	GLuint size = rif.materials.size();
	vector<Material>mats = rif.materials;
	string rangedFile = "Assets/ranged_and_proj.tga";

	//Load models of each projectile
	ball = new GraphicsObject(rManager->projectile, rangedFile, mats[size-5], "projectile");
	shuriken = new GraphicsObject(rManager->projectileLvl2, rangedFile, mats[size-5], "projectile");
	airplane = new GraphicsObject(rManager->projectileLvl3, rangedFile, mats[size-5], "projectile");


	string tFile = "Assets/wall_512_1_05.tga";
	//Load models of each powerup
	GraphicsObject* melee = new GraphicsObject(rManager->powerupMelee, "Assets/melee_1.tga", mats[size-6], "powerup");
	attachments.push_back(melee);

	GraphicsObject* ranged = new GraphicsObject(rManager->powerupRange, rangedFile, mats[size-5], "powerup");
	attachments.push_back(ranged);
	
	GraphicsObject* shield = new GraphicsObject(rManager->powerupShield, "Assets/shield_1.tga", mats[size-5], "powerup");
	attachments.push_back(shield);
	
	GraphicsObject* melee2 = new GraphicsObject(rManager->powerupMeleeLvl2, "Assets/melee_2.tga", mats[size-6], "powerup");
	attachments.push_back(melee2);

	GraphicsObject* ranged2 = new GraphicsObject(rManager->powerupRangeLvl2, rangedFile, mats[size-5], "powerup");
	attachments.push_back(ranged2);

	GraphicsObject* shield2 = new GraphicsObject(rManager->powerupShieldLvl2, "Assets/shield_2.tga", mats[size-5], "powerup");
	attachments.push_back(shield2);

	GraphicsObject* melee3 = new GraphicsObject(rManager->powerupMeleeLvl3, "Assets/melee_3.tga", mats[size-6], "powerup");
	attachments.push_back(melee3);
	
	GraphicsObject* ranged3 = new GraphicsObject(rManager->powerupRangeLvl3, rangedFile, mats[size-5], "powerup");
	attachments.push_back(ranged3);

	GraphicsObject* shield3 = new GraphicsObject(rManager->powerupShieldLvl3, "Assets/shield_3.tga", mats[size-5], "powerup");
	attachments.push_back(shield3);
	
	mainCamera = new Camera();
	mainCamera->setup(gObjList[0]->rotationQuat, roombaPosition);
}

void Renderer::gameOverState()
{
		roombaPosition = vec3(0.0f, -20.0f, 0.0f);
		health = 0;
		pLevel = -1;
		pType = 0;
		kills = -1;
		gameTime = 0;
}

/*
	Method: updatePositions
	-Updates the positions and orientations of the objects in the scene
	-Assumes the dynamic objects are in the first part of the vertex buffer
*/
void Renderer::updateScene()
{
	vector<Entity*> entities = eManager->entityList;
	
	for(GLuint i = 0; i < 8; i++)
	{
		if(((Roomba*)entities[i])->isActivated())
			gObjList[i]->setActive(true);
	}
	for(GLuint i = 0; i < entities.size(); i++)
	{
		if(i == gObjList.size())
		{
			if(entities[i]->getTag() == "powerup")
				gObjList.push_back(powerupList[entities[i]->pIndex]);
			else
			{
				switch (((Projectile*)entities[i])->getDamage())
				{
				case 1: gObjList.push_back(ball); break;
				case 2: gObjList.push_back(shuriken); break;
				case 3: gObjList.push_back(airplane); break;
				}
				
			}
		}
	}
	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		if(strcmp(gObjList[i]->getTag(), "roomba") == 0) 
		{
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation(), ((Roomba*)entities[i])->getPowerupType(), ((Roomba*)entities[i])->getPowerupLevel());
			scoreBoard[i].name = playerNames[i];
			scoreBoard[i].score = ((Roomba*)entities[i])->getKills();
		}
		else if(strcmp(gObjList[i]->getTag(), "airoomba") == 0) 
		{
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation(), ((AIRoomba*)entities[i])->getPowerupType(), ((AIRoomba*)entities[i])->getPowerupLevel());
			scoreBoard[i].name = playerNames[i];
			scoreBoard[i].score = ((Roomba*)entities[i])->getKills();
		}
		else
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation());
	}

	if(eManager->roombas[0]->getKills() < KILLS_TO_WIN && gameOver < 0)
	{
		roombaPosition = entities[0]->getPosition();
		health = (GLfloat)eManager->roombas[0]->getHealth();
		pLevel = eManager->roombas[0]->getPowerupLevel();
		kills = eManager->roombas[0]->getKills();
		pType = eManager->roombas[0]->getPowerupType();
	}
}


void Renderer::drawScene(int width, int height)
{
	mainCamera->update(timestep, gObjList[0]->rotationQuat, roombaPosition);
	modelView = lookAt(mainCamera->getPosition(), mainCamera->getTarget(), mainCamera->getUp());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	skybox->renderSkybox(projection, modelView);
	glUseProgram(shaderProgram);

	glUniformMatrix4fv (shaderIDs[projMat], 1, GL_FALSE, value_ptr (projection));
	glUniform3f (shaderIDs[lightPos], 5.0f, 50.0f, 0.0f);

	for(GLuint i = 0; i < staticList.size(); i++)
	{
		staticList[i]->draw(modelView, shaderIDs);
	}
	
	GLuint pow;
	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		if(gObjList[i]->isActive())
		{ 
			gObjList[i]->draw(modelView, shaderIDs);

			pow = gObjList[i]->getActivePow();
			if(pow > 0 && pow < 10)
				attachments[pow-1]->draw(modelView, shaderIDs, gObjList[i]->translateVector, gObjList[i]->rotationQuat);
		}
	}

	if(!gObjList[0]->isActive())
		gui->showRespawning();
	else
		gui->hideRespawning();

	gui->drawStaticElements(gameOver);
	gui->drawDynamicElements(gameTime, pLevel, pType, kills, health, scoreBoard, gameOver);

	/*
	gui->drawMenu(MAIN_MENU);
	gui->drawMenu(PAUSE_MENU);
	gui->drawMenu(GAME_OVER_MENU);
	*/
}

void Renderer::menu(int _menu, int pos)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	if(glfwWindowShouldClose(window)){
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);		// It's dirty, but it works.
	}
	if (_menu == 0)
		gui->drawMenu(MAIN_MENU, pos);
	else if(_menu == 1)
		gui->drawMenu(PAUSE_MENU, pos);
	//else if (_menu == 2)
		//gui->drawMenu(INSTRUCTIONS);
	else
	{
		gui->drawMenu(GAME_OVER_MENU, pos);
		gui->drawTopScores(scoreBoard, 10.0f, 10.0f, scoreBoard.size());
	}
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::Update()
{
	this->eManager = EntityManager::mainEntityManager;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	if(glfwWindowShouldClose(window)){
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);		// It's dirty, but it works.
	}

	if(clock() - timeBuffer > CLOCKS_PER_SEC && gameTime > 0)
	{
		gameTime -= 1;
		timeBuffer = (GLfloat)clock();
	}
	
	if(gameTime > 0 && eManager->roombas[0]->getKills() >= KILLS_TO_WIN && !justEnded)
	{
		gameOver = 1;
		justEnded = true;
	}
	else if (gameTime == 0 && !justEnded)
	{
		scoreBoard = gui->sortScores(scoreBoard);
		if (scoreBoard[0].name.compare("Player One") == 0)
			gameOver = 1;
		else
			gameOver = 2;
		justEnded = true;
	}


	if(gameOver > 0)
		gameOverState();

	updateScene();
	drawScene(width, height);
	destroyObjects();

	// Note that buffer swapping and polling for events is done here so please don't do it in the function used to draw the scene.
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::destroyObjects()
{
	GLuint index = 0;
	for(GLuint i = 0; i < eManager->entityList.size(); i++)
	{
		if(eManager->entityList[i]->isDestroyed() < 0)
		{
			if(strcmp(gObjList[index]->getTag(), "roomba") == 0 || strcmp(gObjList[index]->getTag(), "airoomba") == 0)
			{
				gObjList[index]->setActive(false);
				gObjList[index]->setActivePow(NO_POWERUP);
			}
			else
				gObjList.erase(gObjList.begin() + index--);
		}
		index++;
	}
}

void Renderer::clearObjData()
{
	for(GLuint i = 0; i < gObjList.size(); i++)
		gObjList[i]->clear();
}

GLFWwindow* Renderer::getWindow(){
	return window;
}

GLboolean Renderer::readShader(const char* filename, int shaderType)
{
	GLint shaderLength = 0;
	FILE* sFile;

	fopen_s(&sFile, filename, "r");
	if(sFile != NULL) {
		
		while (fgetc(sFile) != EOF)
			shaderLength++;

		rewind(sFile);
		fread(shaderText, 1, shaderLength, sFile);
		shaderText[shaderLength] = '\0';

		fclose(sFile);
	}
	else {
		return false;
	}
  
	if(shaderType == VERTEX)
		vertexShaderFile[0] = (GLchar*)((const char*)shaderText);
	else
		fragmentShaderFile[0] = (GLchar*)((const char*)shaderText);
  
	return true;
}

GLint Renderer::setupShaders()
{
	vertShaderPtr = glCreateShader(GL_VERTEX_SHADER);
	fragShaderPtr = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = (GLuint)NULL;
  
	if( !readShader(vsFilename, VERTEX) ) {
		glDeleteShader( vertShaderPtr );
		glDeleteShader( fragShaderPtr );
		std::cout << "The shader " << vsFilename << " not found.\n";
	}
	else
		glShaderSource(vertShaderPtr, 1, (const GLchar**)vertexShaderFile, NULL );
  
	if( !readShader(fsFilename, FRAGMENT) ) {
		glDeleteShader( vertShaderPtr );
		glDeleteShader( fragShaderPtr );
		std::cout << "The shader " << fsFilename << " not found.\n";
	}
	else
		glShaderSource(fragShaderPtr, 1, (const GLchar**)fragmentShaderFile, NULL );
  
	glCompileShader(vertShaderPtr);
	glCompileShader(fragShaderPtr);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShaderPtr);
	glAttachShader(shaderProgram, fragShaderPtr);

	glBindAttribLocation( shaderProgram, VERTEX_DATA, "position" );
	glBindAttribLocation( shaderProgram, NORMAL_DATA, "normal" );
	glBindAttribLocation( shaderProgram, TEXTURE_DATA, "tcoords" );

	glLinkProgram(shaderProgram);

	shaderIDs[ambient] = glGetUniformLocation(shaderProgram, "ambient");
	shaderIDs[diffuse] = glGetUniformLocation(shaderProgram, "diffuse_albedo");
	shaderIDs[specAlb] = glGetUniformLocation(shaderProgram, "specular_albedo");
	shaderIDs[specPow] = glGetUniformLocation(shaderProgram, "specular_power");
	shaderIDs[texObj] = glGetUniformLocation(shaderProgram, "texObject");
	shaderIDs[mvMat] = glGetUniformLocation(shaderProgram, "mv_matrix");
	shaderIDs[projMat] = glGetUniformLocation(shaderProgram, "proj_matrix");
	shaderIDs[lightPos] = glGetUniformLocation(shaderProgram, "light_pos");

	projection = perspective (60.0f, (float)wWidth / (float)wHeight, 0.1f, 1000.0f);

	glDeleteShader(vertShaderPtr);
	glDeleteShader(fragShaderPtr);

	return 0;
}
