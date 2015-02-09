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
	rendering each object. For right now I'm using hard coded rotation and translation vectors
	since my method for extracting and determining the translation and rotation vectors isn't
	working. I'm not sure what to do about the scale vectors for each object so any input would be
	appreciated.
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
		setupScene();
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

void Renderer::setupScene(){
	vector<Entity> entities = eManager->entityList;
	vector<StaticObject> sObjects = eManager->staticList;
	vector<GLfloat> vertBuffer, normBuffer;
	vector<GLuint> faceBuffer, nIndBuffer;
	StaticObject staticBuffer;
	Entity entityBuffer;
	obj *objBuffer;
	vec3 scale, translate, pBuffer;
	quat rotateBuffer;

	vertices.resize(0);
	normals.resize(0);
	faceIndices.resize(0);
	normIndices.resize(0);
	faceSizes.resize(0);
	rotateQuats.resize(0);
	scaleVectors.resize(0);
	transVectors.resize(0);
	colorVectors.resize(0);
	
	objBuffer = (obj*)malloc(sizeof(obj));

	for(GLuint i = 0; i < entities.size(); i++)
	{
		entityBuffer = entities[0];
		objBuffer = entityBuffer.getModel();

		vBuffer = *objBuffer->vertices;//Load vertices of obj to be rearranged
		nBuffer = *objBuffer->normals;//Load normals of obj to be rearranged
		faceBuffer = *objBuffer->faceIndices;
		nIndBuffer = *objBuffer->normIndices;

		vertBuffer = rearrangeVerts(faceBuffer);//rearrange vertices of object
		normBuffer = rearrangeNorms(nIndBuffer);//rearrange normals of object
		vertices.insert(vertices.end(), vertBuffer.begin(), vertBuffer.end());//Concatenate the rearranged vertices into the buffer that will be sent down the pipeline
		normals.insert(normals.end(), normBuffer.begin(), normBuffer.end());//Concatenate the rearanged normals into the buffer that will be sent down the pipeline

		pBuffer = vec3(vertBuffer[0], vertBuffer[1], vertBuffer[2]);//Idea is to get an arbitrary vertex from the object
		translate = normalize(entityBuffer.getPosition() - pBuffer);//Idea is to use arbitrary vertex to determine the translation vector
		rotateBuffer = entityBuffer.getRotation();//Fetch the rotation quat to be used as rotation vector and angle
		scale = vec3(1.0f);//Not sure what to do about proper scaling of the objects

		scaleVectors.push_back(scale);
		transVectors.push_back(translate);
		rotateQuats.push_back(rotateBuffer);
		faceSizes.push_back(faceBuffer.size());
		colorVectors.push_back(vec3(1.0f, 0.0f, 0.0f));
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		staticBuffer = sObjects[0];
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
		rotateBuffer = staticBuffer.getRotation(); //Fetch the rotation quat to be used as rotation vector and angle
		scale = vec3(1.0f); //Not sure what to do about proper scaling of the objects

		//	 Idea here is to push these transformation vectors per object in order so they
		// can be easily located when determining the modelview matrix of each object
		scaleVectors.push_back(scale);
		transVectors.push_back(translate);
		rotateQuats.push_back(rotateBuffer);
		faceSizes.push_back(faceBuffer.size());
		colorVectors.push_back(vec3(0.0f, 0.0f, 1.0f));
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

void Renderer::buffer()
{
	vector<Entity> entities = eManager->entityList;
	vector<StaticObject> sObjects = eManager->staticList;
	vector<GLfloat> vertBuffer, normBuffer;
	StaticObject staticBuffer;
	Entity entityBuffer;
	obj *objBuffer;
	vec3 scale, translate, pBuffer;
	quat rotateBuffer;

	rotateQuats.resize(0);
	transVectors.resize(0);

	objBuffer = (obj*)malloc(sizeof(obj));
	
	for(GLuint i = 0; i < entities.size(); i++)
	{
		entityBuffer = entities[0];
		objBuffer = entityBuffer.getModel();

		vBuffer = *objBuffer->vertices;
		pBuffer = vec3(vBuffer[0], vBuffer[1], vBuffer[2]);//Idea is to get an arbitrary vertex from the object
		translate = normalize(entityBuffer.getPosition() - pBuffer);//Idea is to use arbitrary vertex to determine the translation vector
		rotateBuffer = entityBuffer.getRotation();//Fetch the rotation quat to be used as rotation vector and angle

		transVectors.push_back(translate);
		rotateQuats.push_back(rotateBuffer);
	}

	for(GLuint i = 0; i < sObjects.size(); i++)
	{
		staticBuffer = sObjects[0];
		objBuffer = staticBuffer.getModel();

		vBuffer = *objBuffer->vertices;
		pBuffer = vec3(vBuffer[0], vBuffer[1], vBuffer[2]); //Idea is to get an arbitrary vertex from the object
		translate = staticBuffer.getPosition(); //Idea is to use arbitrary vertex to determine the translation vector
		rotateBuffer = staticBuffer.getRotation(); //Fetch the rotation quat to be used as rotation vector and angle

		//	 Idea here is to push these transformation vectors per object in order so they
		// can be easily located when determining the modelview matrix of each object
		transVectors.push_back(translate);
		rotateQuats.push_back(rotateBuffer);
	}

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
void Renderer::drawObject(vec3 translate, vec3 scale, quat rotate, vec3 color, GLint start, GLsizei count)
{
	mat4 transform(1.0f);

	transform = glm::translate(modelView, translate);
	transform = glm::scale(transform, scale);
	transform *= mat4_cast(rotate);

	glUniform3f(glGetUniformLocation(shaderProgram, "ambient"), color.x, color.y, color.z); //Sets ambient lighting colour
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mv_matrix"), 1, GL_FALSE, value_ptr(transform));
	
	glDrawArrays(GL_TRIANGLES, start, count);
}

void Renderer::drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);

	modelView = lookAt(vec3(0.0f, 95.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = perspective (45.0f, (float)1024 / (float)768, 0.1f, 100.0f);

	glUniformMatrix4fv (glGetUniformLocation(shaderProgram, "proj_matrix"), 
		1, GL_FALSE, value_ptr (projection));

	glUniform3f (glGetUniformLocation(shaderProgram, "light_pos"), 1.0f, 1.0f, -1.0f);

	glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
  
	glEnableVertexAttribArray (VERTEX_DATA);
	glVertexAttribPointer (VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glEnableVertexAttribArray (NORMAL_DATA);
	glVertexAttribPointer (NORMAL_DATA, 3, GL_FLOAT, GL_FALSE, 0,
		(const GLvoid*)(sizeof(GLfloat) * vertices.size()));

	for(int i = 0; i < faceSizes.size(); i++){
		if(i == 0)
		{
			drawObject(transVectors[i], scaleVectors[i], rotateQuats[i], colorVectors[i], 0, faceSizes[i]);
		}
		else{
			drawObject(transVectors[i], scaleVectors[i], rotateQuats[i], colorVectors[i],faceSizes[i-1], faceSizes[i]);
		}
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
	}
	this->eManager = eManager;
	buffer();
	drawScene();
	// Note that buffer swapping and polling for events is done here so please don't do it in the function used to draw the scene.
	glfwSwapBuffers(window);
	glfwPollEvents();
}