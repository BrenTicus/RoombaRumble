#pragma once

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "Entity.h"
#include <vector>
#include <string>

class PowerupObject{
private:
	std::vector<GLfloat> vertices, normals, texVertices;
	std::vector<GLuint> indices, normIndices, texIndices;
	GLuint numIndices;

public:
	GLuint type, level;
	Entity e;
	obj* model;

	GLubyte* tgaBits;
	GLuint tWidth, tHeight, tComponents;
	GLenum eFormat;
	std::string textureFile;
	GLuint VAO, TBO, VBO;
	glm::vec3 max, min, center;

	PowerupObject();
	~PowerupObject();

	void load(GLuint t, GLuint lvl);
	GLuint getBufferID(GLuint bufferType);
	void moveObjToPow();
	void bindPowBuffer();
	void genPowBuffer();
	bool loadTGATexture(GLenum minFilter,
						GLenum magFilter, GLenum wrapMode);

	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	GLuint getNumIndices();
	GLuint bufferSize();
	void findMax();
	void findMin();
	void findCenter();
	void rearrangeData();
	GLboolean readTGABits();
	void clearData();
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