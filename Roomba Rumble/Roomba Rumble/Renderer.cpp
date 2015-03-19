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

#define VA 1
#define VB 2
#define TB 3

#define NO_UPGRADE 0
#define MELEE 1
#define RANGED 2
#define DEFENSE 3

static GLubyte shaderText[MAX_SHADER_SIZE];
char* vsFilename = "vertPhong.vs.glsl";
char* fsFilename = "fragPhong.fs.glsl";

GLuint shaderIDs[8];

Renderer::Renderer(EntityManager* eManager)
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

		this->eManager = eManager;
		rif = eManager->getRif();
		rif.getRendererInfo();

		skybox = new Skybox();

		setupShaders();
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

	Material material;

	GLuint rifIndex = 0;

	GLboolean mel = false;
	GLboolean range = false;
	GLboolean shield = false;

	for(GLuint i = 0; i < entities.size(); i++)
	{
		GraphicsObject* gObject = new GraphicsObject(entities[i]->getModel(), rif.textureFileNames[rifIndex]);

		if(i == 0){
			roombaPosition = entities[i]->getPosition();
		}

		gObject->translateVector = entities[i]->getPosition() - gObject->center;//Use center of the object as a reference to find the translation vector
		gObject->rotationQuat = entities[i]->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		gObject->material = rif.materials[rifIndex++];
		gObject->setTag(entities[i]->getTag());

		gObjList.push_back(gObject);

		if(entities[i]->powerupID == "melee" && !mel)
		{
			powerupList.push_back(gObject);
			mel = true;
		}
		if(entities[i]->powerupID == "ranged" && !range)
		{
			powerupList.push_back(gObject);
			range = true;
		}
		if(entities[i]->powerupID == "shield" && !shield)
		{
			powerupList.push_back(gObject);
			shield = true;
		}
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		GraphicsObject* gObject = new GraphicsObject(sObjects[i]->getModel(), rif.textureFileNames[rifIndex]);

		gObject->translateVector = sObjects[i]->getPosition() - gObject->center;//Use center of the object as a reference to find the translation vector
		gObject->rotationQuat = sObjects[i]->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera
		
		gObject->material = rif.materials[rifIndex++];
		gObject->setTag(sObjects[i]->getTag());

		staticList.push_back(gObject);
	}
	Entity e;
	obj* objBuffer = new obj();
	e.readObj(objBuffer, "Assets/projectile.obj");
	
	projectile = new GraphicsObject(objBuffer, "Assets/wall_512_1_05.tga");
	projectile->material = rif.materials[rifIndex-1];
	projectile->setTag("projectile");
}

/*
	Method: updatePositions
	-Updates the positions and orientations of the objects in the scene
	-Assumes the dynamic objects are in the first part of the vertex buffer
*/
void Renderer::updatePositions()
{
	vector<Entity*> entities = eManager->entityList;
	GLuint newPowerup;

	for(GLuint i = 0; i < entities.size(); i++)
	{
		if(entities[i]->getTag() == "projectile")
		{
			if(entities[i]->justAdded)
			{
				gObjList.push_back(projectile);
				entities[i]->justAdded = false;
			}
		}
	}

	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		if(i == 0)
			roombaPosition = entities[0]->getPosition();
			
		newPowerup = entities[i]->powerupType;
		if(gObjList[i]->getActivePow() != newPowerup)
			gObjList[i]->setActivePow(newPowerup);
	
		gObjList[i]->translateVector = entities[i]->getPosition() - gObjList[i]->center; //Use center of the object as a reference to find the translation vector
		gObjList[i]->rotationQuat = entities[i]->getRotation(); //Fetch the rotation quat to be used for object orientation and camera coordinates
	}
}


void Renderer::drawScene(int width, int height)
{
	Camera camera;

	camera.setup(gObjList[0]->rotationQuat, roombaPosition);
	modelView = lookAt(camera.getPosition(), camera.getTarget(), camera.getUp());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		if(pow > 0 && pow < 4)
			powerupList[pow-1]->draw(modelView, shaderIDs, gObjList[i]->translateVector, gObjList[i]->rotationQuat);
	}
}

void Renderer::Update(EntityManager* eManager)
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	if(glfwWindowShouldClose(window)){
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(0);		// It's dirty, but it works.
	}

	this->eManager = eManager;
	updatePositions();
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

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