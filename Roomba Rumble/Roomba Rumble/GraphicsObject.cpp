#include "GraphicsObject.h"

GLfloat* GraphicsObject::getData(GLuint type)
{
	if(type == 0)
		return vertices.data();
	else if(type == 1)
		return normals.data();
	else
		return texVertices.data();
}

GLuint GraphicsObject::getSize(GLuint type)
{
	if(type == 0)
		return sizeof(GLfloat) * vertices.size();
	else if(type == 1)
		return sizeof(GLfloat) * normals.size();
	else
		return sizeof(GLfloat) * texVertices.size();
}

GLuint GraphicsObject::getNumIndices()
{
	return indices.size();
}

GLuint GraphicsObject::bufferSize()
{
	return sizeof(GLfloat) * (vertices.size() + normals.size() + texVertices.size());
}

void GraphicsObject::findMax(){
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

void GraphicsObject::findMin(){
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

void GraphicsObject::findCenter()
{
	glm::vec3 diff, center;

	findMax();
	findMin();

	center.x = (max.x - min.x) / 2;
	center.y = 0.0f;
	center.z = (max.z - min.z) / 2;
}

void GraphicsObject::clear()
{
	vertices.resize(0);
	normals.resize(0);
	texVertices.resize(0);
	indices.resize(0);
	normIndices.resize(0);
	texIndices.resize(0);
}

void GraphicsObject::rearrangeData()
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

GLboolean GraphicsObject::readTGABits()
{
	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long imageSize;
	short sDepth;

	eFormat = GL_BGR;
	tComponents = GL_RGB;
  
	// Attempt to open the file
	fopen_s(&pFile, textureFile.c_str(), "rb");
	if(pFile == NULL)
		return false;
	
	// Read in header (binary)
	fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);
  
	// Get width, height, and depth of texture
	tWidth = tgaHeader.width;
	tHeight = tgaHeader.height;
	sDepth = tgaHeader.bits / 8;

	// Put some validity checks here. Very simply, I only understand
	// or care about 8, 24, or 32 bit targa's.
	if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
		return false;
  
	// Set OpenGL format expected
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

	// Calculate size of image buffer
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

GLuint GraphicsObject::getActivePow()
{
	return activePowerup;
}

void GraphicsObject::setActivePow(GLuint pow)
{
	activePowerup = pow;
}