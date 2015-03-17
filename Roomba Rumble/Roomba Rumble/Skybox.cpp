#include "Skybox.h"

// Code base off of http://antongerdelan.net/opengl/cubemaps.html

#define MAX_SHADER_SIZE 8192
#define VERTEX 0
#define FRAGMENT 1

static GLubyte shadeText[MAX_SHADER_SIZE];
char* vsFile = "skybox.vs.glsl";
char* fsFile = "skybox.fs.glsl";

GLfloat points[] = {
  -200.0f,  200.0f, -200.0f,
  -200.0f, -200.0f, -200.0f,
   200.0f, -200.0f, -200.0f,
   200.0f, -200.0f, -200.0f,
   200.0f,  200.0f, -200.0f,
  -200.0f,  200.0f, -200.0f,
  
  -200.0f, -200.0f,  200.0f,
  -200.0f, -200.0f, -200.0f,
  -200.0f,  200.0f, -200.0f,
  -200.0f,  200.0f, -200.0f,
  -200.0f,  200.0f,  200.0f,
  -200.0f, -200.0f,  200.0f,
  
   200.0f, -200.0f, -200.0f,
   200.0f, -200.0f,  200.0f,
   200.0f,  200.0f,  200.0f,
   200.0f,  200.0f,  200.0f,
   200.0f,  200.0f, -200.0f,
   200.0f, -200.0f, -200.0f,
   
  -200.0f, -200.0f,  200.0f,
  -200.0f,  200.0f,  200.0f,
   200.0f,  200.0f,  200.0f,
   200.0f,  200.0f,  200.0f,
   200.0f, -200.0f,  200.0f,
  -200.0f, -200.0f,  200.0f,
  
  -200.0f,  200.0f, -200.0f,
   200.0f,  200.0f, -200.0f,
   200.0f,  200.0f,  200.0f,
   200.0f,  200.0f,  200.0f,
  -200.0f,  200.0f,  200.0f,
  -200.0f,  200.0f, -200.0f,
  
  -200.0f, -200.0f, -200.0f,
  -200.0f, -200.0f,  200.0f,
   200.0f, -200.0f, -200.0f,
   200.0f, -200.0f, -200.0f,
  -200.0f, -200.0f,  200.0f,
   200.0f, -200.0f,  200.0f
};

Skybox::Skybox()
{
	setupShaders();
	bindBuffers();
	createCubeMap(  "Assets/Skybox/BasicSpaceTexFT.tga",
					"Assets/Skybox/BasicSpaceTexBK.tga",
					"Assets/Skybox/BasicSpaceTexUP.tga",
					"Assets/Skybox/BasicSpaceTexDN.tga",
					"Assets/Skybox/BasicSpaceTexLF.tga",
					"Assets/Skybox/BasicSpaceTexRT.tga",
					&TBO);
}

Skybox::~Skybox()
{
}

void Skybox::renderSkybox(glm::mat4 proj, glm::mat4 mv)
{
	glDepthMask (GL_FALSE);
	glUseProgram (shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mv_matrix"), 1, GL_FALSE, value_ptr(mv));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_CUBE_MAP, TBO);
	glBindVertexArray (VAO);
	glDrawArrays (GL_TRIANGLES, 0, 36);
	glDepthMask (GL_TRUE);
}

void Skybox::bindBuffers()
{
	glGenBuffers (1, &VBO);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), &points, GL_STATIC_DRAW);

	glGenVertexArrays (1, &VAO);
	glBindVertexArray (VAO);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Skybox::createCubeMap (
	const char* front,
	const char* back,
	const char* top,
	const char* bottom,
	const char* left,
	const char* right,
	GLuint* tex_cube)
{
	glActiveTexture (GL_TEXTURE0);
	glGenTextures (1, tex_cube);
  
	// load each image and copy into a side of the cube-map texture
	assert (
	loadCubeMapSide (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
	assert (
	loadCubeMapSide(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
	assert (
	loadCubeMapSide (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
	assert (
	loadCubeMapSide (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
	assert (
	loadCubeMapSide (*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
	assert (
	loadCubeMapSide (*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
	// format cube map texture
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool Skybox::loadCubeMapSide (
  GLuint texture, GLenum side_target, const char* file_name)
{
	glBindTexture (GL_TEXTURE_CUBE_MAP, texture);

	GLuint texComponents;
	GLenum format;
	GLint texWidth, texHeight;
	GLubyte* image_data = readTGABits(file_name, texWidth, texHeight, texComponents, format);
	if (!image_data) {
		std::cout << "Couldn't load: " << file_name << std::endl;
		return false;
	}

	glTexImage2D (side_target, 0, texComponents, texWidth, texHeight,
				0, format, GL_UNSIGNED_BYTE, image_data);

	free (image_data);
	return true;
}
GLubyte* Skybox::readTGABits(const char* fileName, GLint &tWidth, GLint &tHeight, GLuint &texComponents, GLenum &format)
{
	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long imageSize;
	short sDepth;

	format = GL_BGR;
	texComponents = GL_RGB;
  
	fopen_s(&pFile, fileName, "rb");
	if(pFile == NULL)
		return false;
	
	fread(&tgaHeader, 18, 1, pFile);
  
	tWidth = tgaHeader.width;
	tHeight = tgaHeader.height;
	sDepth = tgaHeader.bits / 8;

	if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
		return false;
  
	switch(sDepth)
	{
		case 4:
			format = GL_BGRA;
			texComponents = GL_RGBA;
			break;
		case 1:
			format = GL_LUMINANCE;
			texComponents = GL_LUMINANCE;
			break;
		default:
		break;
	}

	imageSize = tgaHeader.width * tgaHeader.height * sDepth;
	GLubyte* tgaBits = (GLubyte*)malloc(imageSize * sizeof(GLubyte));
  
	if(fread(tgaBits, imageSize, 1, pFile) != 1)
	{
		if(tgaBits != NULL)
			free(tgaBits);
	}
	
	fclose(pFile);

	return tgaBits;
}
bool Skybox::readShader(const char* filename, int shaderType)
{
	GLint shaderLength = 0;
	FILE* sFile;

	fopen_s(&sFile, filename, "r");
	if(sFile != NULL) {
		
		while (fgetc(sFile) != EOF)
			shaderLength++;

		rewind(sFile);
		fread(shadeText, 1, shaderLength, sFile);
		shadeText[shaderLength] = '\0';

		fclose(sFile);
	}
	else {
		return false;
	}
  
	if(shaderType == VERTEX)
		vertexShaderFile[0] = (GLchar*)((const char*)shadeText);
	else
		fragmentShaderFile[0] = (GLchar*)((const char*)shadeText);
  
	return true;
}

int Skybox::setupShaders()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	vertShaderPtr = glCreateShader(GL_VERTEX_SHADER);
	fragShaderPtr = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = (GLuint)NULL;
  
	if( !readShader(vsFile, VERTEX) ) {
		glDeleteShader( vertShaderPtr );
		glDeleteShader( fragShaderPtr );
		std::cout << "The shader " << vsFile << " not found.\n";
	}
	else
		glShaderSource(vertShaderPtr, 1, (const GLchar**)vertexShaderFile, NULL );
  
	if( !readShader(fsFile, FRAGMENT) ) {
		glDeleteShader( vertShaderPtr );
		glDeleteShader( fragShaderPtr );
		std::cout << "The shader " << fsFile << " not found.\n";
	}
	else
		glShaderSource(fragShaderPtr, 1, (const GLchar**)fragmentShaderFile, NULL );
  
	glCompileShader(vertShaderPtr);
	glCompileShader(fragShaderPtr);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShaderPtr);
	glAttachShader(shaderProgram, fragShaderPtr);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertShaderPtr);
	glDeleteShader(fragShaderPtr);

	return 0;
}