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
		setupShaders();
		setupObjectsInScene();
		bindBuffers();
	}
}


Renderer::~Renderer()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

vec4 Renderer::getVertex (int i) {
	return vec4 (vBuffer[4*i], vBuffer[4*i+1], vBuffer[4*i+2], vBuffer[4*i+3]);
}

vec3 Renderer::getNormal (int i) {
	return vec3 (nBuffer[3*i], nBuffer[3*i+1], nBuffer[3*i+2]);
}
\
/*
	rearrangeVerts: 
		-Want to rearrange the vertices so they're loaded into the buffer based on the face indices
*/
vector<GLfloat> Renderer::rearrangeVerts(vector<GLuint> indices)
{
	vector<GLfloat> tempVerts;
	vec4 v;

	for (GLuint i = 0; i < indices.size(); i++){
		v = getVertex(indices[i]);
		tempVerts.push_back(v.x);
		tempVerts.push_back(v.y);
		tempVerts.push_back(v.z);
		tempVerts.push_back(v.w);
	}

	return tempVerts;
}

/*
	rearrangeNorms: 
		-Want to rearrange the normals so they're loaded into the buffer based on the normal indices
*/
vector<GLfloat> Renderer::rearrangeNorms(vector<GLuint> indices)
{
	vector<GLfloat> tempNorms;
	vec3 n;

	for (GLuint i = 0; i < indices.size(); i++){
		n = getNormal(indices[i]);
		tempNorms.push_back(n.x);
		tempNorms.push_back(n.y);
		tempNorms.push_back(n.z);
	}

	return tempNorms;
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
	vector<Entity> entities = eManager->entityList;
	vector<StaticObject> sObjects = eManager->staticList;
	vector<GLfloat> vertBuffer, normBuffer;
	vector<GLuint> faceBuffer, nIndBuffer;
	StaticObject staticBuffer;
	Entity entityBuffer;
	obj *objBuffer;
	Material staticM, dynamicM;
	vec3 translate, pBuffer;
	quat rotateBuffer;

	numDynamicObjects = 0;
	numStaticObjects = 0;

	for(GLuint i = 0; i < entities.size(); i++)
	{
		entityBuffer = entities[i];
		objBuffer = entityBuffer.getModel();

		vBuffer = *objBuffer->vertices;//Load vertices of obj to be rearranged
		nBuffer = *objBuffer->normals;//Load normals of obj to be rearranged
		faceBuffer = *objBuffer->faceIndices;
		nIndBuffer = *objBuffer->normIndices;

		vertBuffer = rearrangeVerts(faceBuffer);//rearrange vertices of object
		normBuffer = rearrangeNorms(nIndBuffer);//rearrange normals of object
		vertices.insert(vertices.end(), vertBuffer.begin(), vertBuffer.end());//Concatenate the rearranged vertices into the buffer that will be sent down the pipeline
		normals.insert(normals.end(), normBuffer.begin(), normBuffer.end());//Concatenate the rearanged normals into the buffer that will be sent down the pipeline

		roombaPosition = entityBuffer.getPosition();
		pBuffer = vec3(vertBuffer[0], vertBuffer[1], vertBuffer[2]);//Idea is to get an arbitrary vertex from the object
		translate = roombaPosition - pBuffer;//Idea is to use arbitrary vertex to determine the translation vector
		rotateBuffer = entityBuffer.getRotation(); //Fetch the rotation quat to be used to orientate objects and position the camera

		dTransVectors.push_back(translate);
		dRotateQuats.push_back(rotateBuffer);
		faceSizes.push_back(faceBuffer.size());

		//Material properties are hard coded here
		dynamicM.ambient = vec3(1.0f, 0.0f, 0.0f);
		dynamicM.diffuseAlbedo = vec3(0.1f);
		dynamicM.specularAlbedo = vec3(0.1f);
		dynamicM.specularPower = 30.0f;
		dMaterials.push_back(dynamicM);

		numDynamicObjects++;
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		staticBuffer = sObjects[i];
		objBuffer = staticBuffer.getModel();

		vBuffer = *objBuffer->vertices; //Load vertices of obj to be rearranged
		nBuffer = *objBuffer->normals; //Load normals of obj to be rearranged
		faceBuffer = *objBuffer->faceIndices; 
		nIndBuffer = *objBuffer->normIndices;

		vertBuffer = rearrangeVerts(faceBuffer); //rearrange vertices of object
		normBuffer = rearrangeNorms(nIndBuffer); //rearrange normals of object

		vertices.insert(vertices.end(), vertBuffer.begin(), vertBuffer.end()); //Concatenate the rearranged vertices into the buffer that will be sent down the pipeline
		normals.insert(normals.end(), normBuffer.begin(), normBuffer.end()); //Concatenate the rearanged normals into the buffer that will be sent down the pipeline

		pBuffer = vec3(vertBuffer[0], vertBuffer[1], vertBuffer[2]); //Idea is to get an arbitrary vertex from the object
		translate = staticBuffer.getPosition(); //Idea is to use arbitrary vertex to determine the translation vector
		rotateBuffer = staticBuffer.getRotation(); 

		//	 Idea here is to push these transformation vectors per object in order so they
		// can be easily located when determining the modelview matrix of each object
		sTransVectors.push_back(translate);
		sRotateQuats.push_back(rotateBuffer);
		faceSizes.push_back(faceBuffer.size());
		
		//Material properties are hard coded here
		staticM.ambient = vec3(0.1f);
		staticM.diffuseAlbedo = vec3(0.1f);
		staticM.specularAlbedo = vec3(0.3f);
		staticM.specularPower = 30.0f;
		sMaterials.push_back(staticM);

		numStaticObjects++;
	}
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

	glLinkProgram(shaderProgram);
	glDeleteShader(vertShaderPtr);
	glDeleteShader(fragShaderPtr);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	return 0;
}

/*
	Method: updatePositions
	-Updates the positions and orientations of the objects in the scene
*/
void Renderer::updatePositions()
{
	vector<Entity> entities = eManager->entityList;
	vector<StaticObject> sObjects = eManager->staticList;
	vector<GLfloat> vertBuffer, normBuffer;
	StaticObject staticBuffer;
	Entity entityBuffer;
	obj *objBuffer;
	vec3 translate, pBuffer;
	quat rotateBuffer;

	dRotateQuats.resize(0);
	dTransVectors.resize(0);
	
	for(GLuint i = 0; i < entities.size(); i++)
	{
		entityBuffer = entities[i]; //Fetch entity to update
		objBuffer = entityBuffer.getModel(); //Fetch coordinates from the object of the entity

		roombaPosition = entityBuffer.getPosition();
		vertBuffer = *objBuffer->vertices;
		pBuffer = vec3(vertBuffer[0], vertBuffer[1], vertBuffer[2]); //Get an arbitrary vertex from the object
		translate = roombaPosition - pBuffer; //Use arbitrary vertex to determine the translation vector
		rotateBuffer = entityBuffer.getRotation(); //Fetch the rotation quat to be used for object orientation and camera coordinates
		
		//	 Idea here is to push these transformation vectors per object in order so they
		// can be easily located when determining the modelview matrix of each object
		dTransVectors.push_back(translate);
		dRotateQuats.push_back(rotateBuffer);
	}
}

void Renderer::bindBuffers()
{
	glGenBuffers (1, &vertexBuffer);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
	
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertices.size() +  sizeof(GLfloat) * normals.size(),
		NULL, 
		GL_STATIC_DRAW);
	glBufferSubData (GL_ARRAY_BUFFER,
		0,
		sizeof(GLfloat) * vertices.size(),
		vertices.data());
	glBufferSubData (GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertices.size(),
		sizeof(GLfloat) * normals.size(),
		normals.data());
}

void Renderer::drawObject(vec3 translate, vec3 scale, quat rotate, Material m, GLint start, GLsizei count)
{
	mat4 transform(1.0f);

	transform = glm::translate(modelView, translate);
	transform = glm::scale(transform, scale);
	transform *= mat4_cast(rotate);

	glUniform3f(glGetUniformLocation(shaderProgram, "ambient"), m.ambient.x, m.ambient.y, m.ambient.z); 
	glUniform3f(glGetUniformLocation(shaderProgram, "diffuse_albedo"), m.diffuseAlbedo.x, m.diffuseAlbedo.y, m.diffuseAlbedo.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "specular_albedo"), m.specularAlbedo.x, m.specularAlbedo.y, m.specularAlbedo.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "specular_power"), 85.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mv_matrix"), 1, GL_FALSE, value_ptr(transform));
	
	glDrawArrays(GL_TRIANGLES, start, count);
}

void Renderer::drawScene(int width, int height)
{
	vec3 cameraPosition, cameraTarget;
	//Below I use rotateQuats[0] for now since I now that's where the roomba's rotation quat is stored
	//cameraPosition = roombaPosition + rotateQuats[0] * vec3(0.0f, 0.0f, -15.0f) + vec3(0.0f, 40.0f, 0.0f); // Overhead camera
	cameraPosition = roombaPosition + dRotateQuats[0] * vec3(0.0f, 2.5f, -7.50f); // Third person camera
	cameraTarget = roombaPosition;
	modelView = lookAt(cameraPosition, cameraTarget, vec3(0.0f, 1.0f, 0.0f));
	projection = perspective (60.0f, (float)width / (float)height, 0.1f, 100.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);

	glUniformMatrix4fv (glGetUniformLocation(shaderProgram, "proj_matrix"), 
		1, GL_FALSE, value_ptr (projection));

	glUniform3f (glGetUniformLocation(shaderProgram, "light_pos"), 10.0f, 10.0f, -1.0f);
  
	glEnableVertexAttribArray (VERTEX_DATA);
	glVertexAttribPointer (VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray (NORMAL_DATA);
	glVertexAttribPointer (NORMAL_DATA, 3, GL_FLOAT, GL_FALSE, 0,
		(const GLvoid*)(sizeof(GLfloat) * vertices.size()));

	//Draw Dynamic Objects
	for(GLuint i = 0; i < numDynamicObjects; i++)
	{
		if(i == 0)
			drawObject(dTransVectors[i], vec3(1.0f), dRotateQuats[i], dMaterials[i], 0, faceSizes[i]);
		else
			drawObject(dTransVectors[i], vec3(1.0f), dRotateQuats[i], dMaterials[i], faceSizes[i-1], faceSizes[i]);
	}

	//Draw Static Objects
	for(GLuint i = 0; i < numStaticObjects; i++)
	{
		drawObject(sTransVectors[i], vec3(1.0f), sRotateQuats[i], sMaterials[i], faceSizes[(i+numDynamicObjects)-1], faceSizes[i+numDynamicObjects]);
	}

	glDisableVertexAttribArray(VERTEX_DATA);
	glDisableVertexAttribArray(NORMAL_DATA);
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

	// Note that buffer swapping and polling for events is done here so please don't do it in the function used to draw the scene.
	glfwSwapBuffers(window);
	glfwPollEvents();
}