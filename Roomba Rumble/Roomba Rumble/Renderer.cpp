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

#define NO_UPGRADE 0

static GLubyte shaderText[MAX_SHADER_SIZE];
char* vsFilename = "vertPhong.vs.glsl";
char* fsFilename = "fragPhong.fs.glsl";

GLuint shaderIDs[8];

Renderer::Renderer()
{
	if (glfwInit())
	{
		glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

		// Open a window and create its OpenGL context 
		window = glfwCreateWindow(1024, 768, "Roomba Rumble", NULL, NULL);
		glfwMakeContextCurrent(window); // Initialize GLEW 

		glewExperimental = GL_TRUE;
		glewInit();

		this->eManager = EntityManager::mainEntityManager;
		rManager = ResourceManager::mainResourceManager;
		rif = eManager->getRif();
		rif.getRendererInfo();

		skybox = new Skybox();

		setupShaders();

		gui = new GUI(1024, 768, shaderIDs);

		setupObjectsInScene();
		clearObjData();
	}
}

Renderer::~Renderer()
{
	glfwDestroyWindow(window);
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
	
	Material mat = rif.materials[rifIndex - 1];
	string tFile = "Assets/wall_512_1_05.tga";


	//Load models of each roomba
	roomba = new GraphicsObject(rManager->roomba, "Assets/roomba.tga", rif.materials[0], "roomba");

	for(GLuint i = 0; i < 10; i++)
	{
		GraphicsObject* airoomba = new GraphicsObject(rManager->roomba, "Assets/airoomba.tga", rif.materials[1], "airoomba");
		aiRoombas.push_back(airoomba);
	}

	//Load models of each projectile
	ball = new GraphicsObject(rManager->projectile, tFile, mat, "projectile");
	shuriken = new GraphicsObject(rManager->projectileLvl2, tFile, mat, "projectile");
	airplane = new GraphicsObject(rManager->projectileLvl3, tFile, mat, "projectile");


	//Load models of each powerup
	GraphicsObject* melee = new GraphicsObject(rManager->powerupMelee, tFile, mat, "powerup");
	attachments.push_back(melee);

	GraphicsObject* ranged = new GraphicsObject(rManager->powerupRange, tFile, mat, "powerup");
	attachments.push_back(ranged);
	
	GraphicsObject* shield = new GraphicsObject(rManager->powerupShield, tFile, mat, "powerup");
	attachments.push_back(shield);
	
	GraphicsObject* melee2 = new GraphicsObject(rManager->powerupMeleeLvl2, tFile, mat, "powerup");
	attachments.push_back(melee2);

	GraphicsObject* ranged2 = new GraphicsObject(rManager->powerupRangeLvl2, tFile, mat, "powerup");
	attachments.push_back(ranged2);

	GraphicsObject* shield2 = new GraphicsObject(rManager->powerupShieldLvl2, tFile, mat, "powerup");
	attachments.push_back(shield2);

	GraphicsObject* melee3 = new GraphicsObject(rManager->powerupMeleeLvl3, tFile, mat, "powerup");
	attachments.push_back(melee3);
	
	GraphicsObject* ranged3 = new GraphicsObject(rManager->powerupRangeLvl3, tFile, mat, "powerup");
	attachments.push_back(ranged3);

	GraphicsObject* shield3 = new GraphicsObject(rManager->powerupShieldLvl3, tFile, mat, "powerup");
	attachments.push_back(shield3);
}

/*
	Method: updatePositions
	-Updates the positions and orientations of the objects in the scene
	-Assumes the dynamic objects are in the first part of the vertex buffer
*/
void Renderer::updateScene()
{
	vector<Entity*> entities = eManager->entityList;
	
	for(GLuint i = 0; i < entities.size(); i++)
	{
		if(i == gObjList.size())
		{
			if(entities[i]->getTag() == "roomba")
				gObjList.push_back(roomba);
			else if(entities[i]->getTag() == "airoomba")
			{
				for(GLuint j = 0; j < 10; j++)
				{
					if(!aiRoombas[j]->isActive())
					{
						aiRoombas[j]->setActive(true);
						aiRoombas[j]->aiIndex = j;
						gObjList.push_back(aiRoombas[j]);
						j = 10;
					}
				}
			}
			else if(entities[i]->getTag() == "powerup")
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
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation(), entities[i]->powerupType, ((Roomba*)entities[i])->getPowerupLevel());
		else if(strcmp(gObjList[i]->getTag(), "airoomba") == 0) 
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation(), entities[i]->powerupType, ((AIRoomba*)entities[i])->getPowerupLevel());
		else
			gObjList[i]->update(entities[i]->getPosition(), entities[i]->getRotation(), entities[i]->powerupType);
	}

	if(eManager->roombas.size() > 0)
	{
		roombaPosition = entities[0]->getPosition();
		health = (GLfloat)eManager->roombas[0]->getHealth();
		damage = eManager->roombas[0]->getDamage();
		kills = eManager->roombas[0]->getKills();
	}
	else
	{
		roombaPosition = vec3(0.0f, -20.0f, 0.0f);
		health = 0;
		damage = -1;
		kills = -1;
	}
}


void Renderer::drawScene(int width, int height)
{
	Camera camera;

	camera.setup(gObjList[0]->rotationQuat, roombaPosition);
	modelView = lookAt(camera.getPosition(), camera.getTarget(), camera.getUp());

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
		gObjList[i]->draw(modelView, shaderIDs);

		pow = gObjList[i]->getActivePow();
		if(pow > 0 && pow < 10)
			attachments[pow-1]->draw(modelView, shaderIDs, gObjList[i]->translateVector, gObjList[i]->rotationQuat);
	}

	gui->drawHealth(health);
	gui->drawStaticElements();
	gui->drawDynamicElements(damage, kills);
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
		if(eManager->entityList[i]->isDestroyed())
		{
			if(gObjList[index]->aiIndex < 11)
				aiRoombas[gObjList[index]->aiIndex]->setActive(false);

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

	projection = perspective (60.0f, (float)1024 / (float)768, 0.1f, 1000.0f);

	glDeleteShader(vertShaderPtr);
	glDeleteShader(fragShaderPtr);

	return 0;
}