#include "GraphicsObject.h"

#define VERTEX_DATA 0
#define NORMAL_DATA 1
#define TEXTURE_DATA 2

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
	return numIndices;
}
void GraphicsObject::setNumIndices()
{
	numIndices = indices.size();
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
	vertices.clear();
	normals.clear();
	texVertices.clear();
	indices.clear();
	normIndices.clear();
	texIndices.clear();
	tgaBits = NULL;
}

void GraphicsObject::clearPowData()
{
	melee.clearData();
	ranged.clearData();
	defense.clearData();
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
void GraphicsObject::bindBuffer()
{
	GLuint offset = 0;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize(), NULL, GL_STATIC_DRAW);
		
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

	//Generate and load texture data
	glGenTextures(1, &TBO);
	glBindTexture(GL_TEXTURE_2D, TBO);

	loadTexture(GL_LINEAR, GL_LINEAR, GL_REPEAT);
}

void GraphicsObject::genBuffer()
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

void GraphicsObject::draw(glm::mat4 modelView, GLuint ambientID, GLuint diffuseID, GLuint specAlbID, GLuint specPowID, GLuint texObjID, GLuint mvMatID)
{
	mat4 transform(1.0f);

	transform = glm::translate(modelView, translateVector);
	transform *= mat4_cast(rotationQuat);

	glUniform3f(ambientID, material.ambient.x, material.ambient.y, material.ambient.z); 
	glUniform3f(diffuseID, material.diffuseAlbedo.x, material.diffuseAlbedo.y, material.diffuseAlbedo.z);
	glUniform3f(specAlbID, material.specularAlbedo.x, material.specularAlbedo.y, material.specularAlbedo.z);
	glUniform1f(specPowID, material.specularPower);
	glUniform1i(texObjID, 0);

	glUniformMatrix4fv(mvMatID, 1, GL_FALSE, value_ptr(transform));
	
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, TBO);
	glDrawArrays(GL_TRIANGLES, 0, getNumIndices());
}

GLboolean GraphicsObject::loadTexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLuint tWidth, tHeight, tComponents;
	GLenum eFormat;

	if(readTGABits(tWidth, tHeight, tComponents, eFormat))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, tComponents, tWidth, tHeight, 0,
			eFormat, GL_UNSIGNED_BYTE, tgaBits);
	}
	else
	{
		cout << "Couldn't read tga texture file " << textureFile << endl;
		return false;
	}

	return true;
}

GLboolean GraphicsObject::readTGABits(GLuint &tWidth, GLuint &tHeight, GLuint &tComponents, GLenum &eFormat)
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