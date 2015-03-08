#include "PowerupObject.h"

#define VERTEX_DATA 0
#define NORMAL_DATA 1
#define TEXTURE_DATA 2

PowerupObject::PowerupObject()
{
}

PowerupObject::~PowerupObject()
{
}

void PowerupObject::load(GLuint t, GLuint lvl)
{
	type = t;
	level = lvl;
	
	string filename;

	if(type == 1)
		filename = "Assets/melee_1.obj";
	else if(type == 2)
		filename = "Assets/ranged_1.obj";
	else if(type == 3)
		filename = "Assets/shield_1.obj";

	textureFile = "Assets/wall_512_1_05.tga";
	model = new obj();
	e.readObj(model, filename);
	moveObjToPow();
	rearrangeData();
	bindPowBuffer();
	genPowBuffer();
	readTGABits();
	loadTGATexture(GL_LINEAR, GL_LINEAR, GL_REPEAT);

	findCenter();
	numIndices = indices.size();
}

GLuint PowerupObject::getBufferID(GLuint bufferType)
{
	if(bufferType == 1)
		return VAO;
	else if(bufferType == 2)
		return VBO;
	else if(bufferType == 3)
		return TBO;
}
void PowerupObject::moveObjToPow()
{
	vertices = *model->vertices;
	normals = *model->normals;
	texVertices = *model->texVertices;

	indices = *model->faceIndices;
	normIndices = *model->normIndices;
	texIndices = *model->texIndices;

	model = NULL;
}
void PowerupObject::bindPowBuffer()
{
	GLuint offset = 0;
	GLuint bSize = bufferSize();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bSize, NULL, GL_STATIC_DRAW);
		
	glBufferSubData (GL_ARRAY_BUFFER,
		offset,
		getSize(VERTEX_DATA),
		getData(VERTEX_DATA));

	offset += getSize(VERTEX_DATA);

	glBufferSubData (GL_ARRAY_BUFFER,
		offset,
		getSize(NORMAL_DATA),
		getData(NORMAL_DATA));

	offset += getSize(NORMAL_DATA);

	glBufferSubData(GL_ARRAY_BUFFER, 
		offset, 
		getSize(TEXTURE_DATA), 
		getData(TEXTURE_DATA));

	glGenTextures(1, &TBO);
	glBindTexture(GL_TEXTURE_2D, TBO);

	loadTGATexture(GL_LINEAR, GL_LINEAR, GL_REPEAT);
}
void PowerupObject::genPowBuffer()
{
	GLuint offset = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(VERTEX_DATA);
	glEnableVertexAttribArray(NORMAL_DATA);
	glEnableVertexAttribArray(TEXTURE_DATA);

	glVertexAttribPointer(VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);

	offset += getSize(VERTEX_DATA);

	glVertexAttribPointer(NORMAL_DATA, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);

	offset += getSize(NORMAL_DATA);

	glVertexAttribPointer(TEXTURE_DATA, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
}
bool PowerupObject::loadTGATexture(GLenum minFilter,
	GLenum magFilter, GLenum wrapMode)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, tComponents, tWidth, tHeight, 0,
		eFormat, GL_UNSIGNED_BYTE, tgaBits);
	
	return true;
}

GLfloat* PowerupObject::getData(GLuint type)
{
	if(type == 0)
		return vertices.data();
	else if(type == 1)
		return normals.data();
	else
		return texVertices.data();
}

GLuint PowerupObject::getSize(GLuint type)
{
	if(type == 0)
		return sizeof(GLfloat) * vertices.size();
	else if(type == 1)
		return sizeof(GLfloat) * normals.size();
	else
		return sizeof(GLfloat) * texVertices.size();
}

GLuint PowerupObject::getNumIndices()
{
	return numIndices;
}

GLuint PowerupObject::bufferSize()
{
	return sizeof(GLfloat) * (vertices.size() + normals.size() + texVertices.size());
}

void PowerupObject::findMax(){
	max.x = vertices[0];
	max.y = vertices[1];
	max.z = vertices[2];

	for(int i = 4; i < vertices.size(); i += 4){
		if(vertices[i] > max.x)
			max.x = vertices[i];

		if(vertices[i + 1] > max.y)
			max.y = vertices[i + 1];

		if(vertices[i + 2] > max.z)
			max.z = vertices[i + 2];
	}
}

void PowerupObject::findMin(){
	min.x = vertices[0];
	min.y = vertices[1];
	min.z = vertices[2];

	for(int i = 4; i < vertices.size(); i += 4){
		if(vertices[i] < min.x)
			min.x = vertices[i];

		if(vertices[i + 1] < min.y)
			min.y = vertices[i + 1];

		if(vertices[i + 2] < min.z)
			min.z = vertices[i + 2];
	}
}

void PowerupObject::findCenter()
{
	glm::vec3 diff, center;

	findMax();
	findMin();

	center.x = (max.x - min.x) / 2;
	center.y = 0.0f;
	center.z = (max.z - min.z) / 2;
}

void PowerupObject::rearrangeData()
{
	std::vector<GLfloat> verts, norms, tex;
	GLuint vi, ni, ti;

	for (int i = 0; i < indices.size(); i++){
		vi = indices[i] * 4;
		verts.push_back(vertices[vi]);
		verts.push_back(vertices[vi+1]);
		verts.push_back(vertices[vi+2]);
		verts.push_back(vertices[vi+3]);

		ni = normIndices[i] * 3;
		norms.push_back(normals[ni]);
		norms.push_back(normals[ni+1]);
		norms.push_back(normals[ni+2]);

		ti = texIndices[i] * 2;
		tex.push_back(texVertices[ti]);
		tex.push_back(texVertices[ti+1]);
	}

	vertices = verts;
	normals = norms;
	texVertices = tex;
}

GLboolean PowerupObject::readTGABits()
{
	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long imageSize;
	short sDepth;

	eFormat = GL_BGR;
	tComponents = GL_RGB;
  
	fopen_s(&pFile, textureFile.c_str(), "rb");
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
			eFormat = GL_BGRA;
			tComponents = GL_RGBA;
			break;
		case 1:
			eFormat = GL_LUMINANCE;
			tComponents = GL_LUMINANCE;
			break;
		default:
		break;
	}

	imageSize = tgaHeader.width * tgaHeader.height * sDepth;
	tgaBits = (GLubyte*)malloc(imageSize * sizeof(GLubyte));
  
	if(fread(tgaBits, imageSize, 1, pFile) != 1)
	{
		if(tgaBits != NULL)
			free(tgaBits);
	}
	
	fclose(pFile);

	return true;
}

void PowerupObject::clearData()
{
	vertices.clear();
	normals.clear();
	texVertices.clear();
	indices.clear();
	normIndices.clear();
	texIndices.clear();
	tgaBits = NULL;
}