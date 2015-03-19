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

bool Renderer::readShader(const char* filename, int shaderType)
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

int Renderer::setupShaders()
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

	ambientID = glGetUniformLocation(shaderProgram, "ambient");
	diffuseID = glGetUniformLocation(shaderProgram, "diffuse_albedo");
	specAlbID = glGetUniformLocation(shaderProgram, "specular_albedo");
	specPowID = glGetUniformLocation(shaderProgram, "specular_power");
	texObjID = glGetUniformLocation(shaderProgram, "texObject");
	mvMatID = glGetUniformLocation(shaderProgram, "mv_matrix");
	projMatID = glGetUniformLocation(shaderProgram, "proj_matrix");
	lightPosID = glGetUniformLocation(shaderProgram, "light_pos");

	projection = perspective (60.0f, (float)1024 / (float)768, 0.1f, 1000.0f);

	glDeleteShader(vertShaderPtr);
	glDeleteShader(fragShaderPtr);

	return 0;
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
	
	GraphicsObject gObject;
	StaticObject* staticBuffer;
	Entity* entityBuffer;
	obj *objBuffer;
	Material material;

	GLuint rifIndex = 0;

	numStatObjs = 0;
	for(GLuint i = 0; i < entities.size(); i++)
	{
		entityBuffer = entities[i];
		objBuffer = entityBuffer->getModel();

		gObject.vertices = *objBuffer->vertices;//Load vertices of obj to be rearranged
		gObject.normals = *objBuffer->normals;//Load normals of obj to be rearranged
		gObject.texVertices = *objBuffer->texVertices;

		gObject.indices = *objBuffer->faceIndices;
		gObject.normIndices = *objBuffer->normIndices;
		gObject.texIndices = *objBuffer->texIndices;

		gObject.rearrangeData();
		gObject.findCenter();

		if(i == 0){
			roombaPosition = entityBuffer->getPosition();
		}

		gObject.textureFile = rif.textureFileNames[rifIndex];
		gObject.bindBuffer();
		gObject.genBuffer();

		gObject.translateVector = entityBuffer->getPosition() - gObject.center;//Use center of the object as a reference to find the translation vector
		gObject.rotationQuat = entityBuffer->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		gObject.material = rif.materials[rifIndex];

		gObject.setAlive(true);
		gObject.setTag(entityBuffer->getTag());
		
		gObject.setActivePow(NO_UPGRADE);
		if(gObject.getTag() == "roomba" || gObject.getTag() == "airoomba")
		{
			gObject.melee.load(MELEE, 1);
			gObject.ranged.load(RANGED, 1);
			gObject.defense.load(DEFENSE, 1);
			gObject.clearPowData();
		}

		gObject.setNumIndices();
		gObjList.push_back(gObject);
		gObject.clear();

		rifIndex++;
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		staticBuffer = sObjects[i];
		objBuffer = staticBuffer->getModel();

		gObject.vertices = *objBuffer->vertices;//Load vertices of obj to be rearranged
		gObject.normals = *objBuffer->normals;//Load normals of obj to be rearranged
		gObject.texVertices = *objBuffer->texVertices;
		
		gObject.indices = *objBuffer->faceIndices;
		gObject.normIndices = *objBuffer->normIndices;
		gObject.texIndices = *objBuffer->texIndices;

		gObject.rearrangeData();
		gObject.findCenter();

		gObject.translateVector = staticBuffer->getPosition() - gObject.center;//Use center of the object as a reference to find the translation vector
		gObject.rotationQuat = staticBuffer->getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		gObject.textureFile = rif.textureFileNames[rifIndex];
		gObject.bindBuffer();
		gObject.genBuffer();
		
		gObject.material = rif.materials[rifIndex];
		gObject.setAlive(true);
		gObject.setTag(staticBuffer->getTag());

		gObject.setActivePow(NO_UPGRADE);

		gObject.setNumIndices();
		staticList.push_back(gObject);
		gObject.clear();

		rifIndex++;
		numStatObjs++;
	}
	Entity e;
	objBuffer = new obj();
	e.readObj(objBuffer, "Assets/projectile_1.obj");

	projectile.vertices = *objBuffer->vertices;//Load vertices of obj to be rearranged
	projectile.normals = *objBuffer->normals;//Load normals of obj to be rearranged
	projectile.texVertices = *objBuffer->texVertices;
		
	projectile.indices = *objBuffer->faceIndices;
	projectile.normIndices = *objBuffer->normIndices;
	projectile.texIndices = *objBuffer->texIndices;
	projectile.rearrangeData();
	projectile.findCenter();
	projectile.material = rif.materials[rifIndex-1];
	projectile.textureFile = "Assets/wall_512_1_05.tga";
	projectile.setAlive(true);
	projectile.setTag("projectile");
	projectile.setActivePow(NO_UPGRADE);
	projectile.setNumIndices();
}

void Renderer::addProjToScene()
{
	GLuint offset = 0;
	
	projectile.bindBuffer();
	projectile.genBuffer();
	
	gObjList.push_back(projectile);
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
				addProjToScene();
				entities[i]->justAdded = false;
			}
		}
	}

	for(GLuint i = 0; i < entities.size(); i++)
	{
		if (entities[i]->isDestroyed()) 
		{
			gObjList[i].destroy();
		}
	}

	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		if(gObjList[i].getTag() == "roomba")
		{
			if(i == 0)
				roombaPosition = entities[0]->getPosition();
			
			newPowerup = entities[i]->powerupType;
			if(gObjList[i].getActivePow() != newPowerup)
				gObjList[i].setActivePow(newPowerup);
		}
		else if(gObjList[i].getTag() == "airoomba")
		{
			newPowerup = entities[i]->powerupType;
			if(gObjList[i].getActivePow() != newPowerup)
				gObjList[i].setActivePow(newPowerup);
		}
		if(entities.size() == gObjList.size())
		{
			gObjList[i].translateVector = entities[i]->getPosition() - gObjList[i].center; //Use center of the object as a reference to find the translation vector
			gObjList[i].rotationQuat = entities[i]->getRotation(); //Fetch the rotation quat to be used for object orientation and camera coordinates
		}
	}
}


void Renderer::drawScene(int width, int height)
{
	Camera camera;
	GLuint pow = 0;

	camera.setup(gObjList[0].rotationQuat, roombaPosition);
	modelView = lookAt(camera.getPosition(), camera.getTarget(), camera.getUp());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	skybox->renderSkybox(projection, modelView);
	glUseProgram(shaderProgram);

	glUniformMatrix4fv (projMatID, 1, GL_FALSE, value_ptr (projection));
	glUniform3f (lightPosID, 5.0f, 50.0f, 0.0f);

	for(GLuint i = 0; i < numStatObjs; i++)
	{
		staticList[i].draw(modelView, ambientID, diffuseID, 
						specAlbID, specPowID, texObjID, mvMatID);
	}

	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		gObjList[i].draw(modelView, ambientID, diffuseID, 
						specAlbID, specPowID, texObjID, mvMatID);

		pow = gObjList[i].getActivePow();
		if(pow > 0)
		{
			if(pow == MELEE)
			{
				glBindVertexArray(gObjList[i].melee.getBufferID(VA));
				glBindTexture(GL_TEXTURE_2D, gObjList[i].melee.getBufferID(TB));
				drawObject(&gObjList[i], vec3(1.0f), gObjList[i].melee.getNumIndices());
			}
			else if(pow == RANGED)
			{
				glBindVertexArray(gObjList[i].ranged.getBufferID(VA));
				glBindTexture(GL_TEXTURE_2D, gObjList[i].ranged.getBufferID(TB));
				drawObject(&gObjList[i], vec3(1.0f), gObjList[i].ranged.getNumIndices());
			}
			else if(pow == DEFENSE)
			{
				glBindVertexArray(gObjList[i].defense.getBufferID(VA));
				glBindTexture(GL_TEXTURE_2D, gObjList[i].defense.getBufferID(TB));
				drawObject(&gObjList[i], vec3(1.0f), gObjList[i].defense.getNumIndices());
			}
		}
	}
}

void Renderer::drawObject(GraphicsObject * gObj, vec3 scale, GLsizei count)
{
	mat4 transform(1.0f);
	Material m = gObj->material;

	transform = glm::translate(modelView, gObj->translateVector);
	transform = glm::scale(transform, scale);
	transform *= mat4_cast(gObj->rotationQuat);

	glUniform3f(ambientID, m.ambient.x, m.ambient.y, m.ambient.z); 
	glUniform3f(diffuseID, m.diffuseAlbedo.x, m.diffuseAlbedo.y, m.diffuseAlbedo.z);
	glUniform3f(specAlbID, m.specularAlbedo.x, m.specularAlbedo.y, m.specularAlbedo.z);
	glUniform1f(specPowID, m.specularPower);
	glUniform1i(texObjID, 0);

	glUniformMatrix4fv(mvMatID, 1, GL_FALSE, value_ptr(transform));
	
	glDrawArrays(GL_TRIANGLES, 0, count);
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
	for(GLuint i = 0; i < gObjList.size(); i++)
	{
		if(!gObjList[i].isAlive())
			gObjList.erase(gObjList.begin() + i);
	}
}

void Renderer::clearObjData()
{
	for(GLuint i = 0; i < gObjList.size(); i++)
		gObjList[i].clear();
}

GLFWwindow* Renderer::getWindow(){
	return window;
}