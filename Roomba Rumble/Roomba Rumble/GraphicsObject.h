#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Material.h"
#include <vector>

class GraphicsObject{
private:
	bool active;
	const char* tag;

public:
	std::vector<GLfloat> vertices, normals, texVertices;
	std::vector<GLuint> indices, normIndices, texIndices;
	glm::quat rotationQuat;
	glm::vec3 translateVector;
	Material material;
	GLubyte* tgaBits;
	GLuint tWidth, tHeight, tComponents;
	GLenum eFormat;
	std::string textureFile;
	GLuint VAO, TBO, VBO;
	glm::vec3 max, min, center;
	
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	GLuint getNumIndices();
	GLuint bufferSize();
	void findMax();
	void findMin();
	void findCenter();

	bool isActive();
	void setActive(bool b);
	void setTag(const char* newTag){ tag = newTag;}
	const char* getTag(){ return tag;}

	void clear();
	void rearrangeData();
	GLboolean readTGABits();
};

#pragma pack(1)
typedef struct
{
  GLbyte	identsize;              // Size of ID field that follows header (0)
  GLbyte	colorMapType;           // 0 = None, 1 = paletted
  GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
  unsigned short	colorMapStart;          // First colour map entry
  unsigned short	colorMapLength;         // Number of colors
  unsigned char 	colorMapBits;   // bits per palette entry
  unsigned short	xstart;                 // image x origin
  unsigned short	ystart;                 // image y origin
  unsigned short	width;                  // width in pixels
  unsigned short	height;                 // height in pixels
  GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
  GLbyte	descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)