#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Entity.h"
#include "Material.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum shaderLocationIDs{
	ambient = 0,
	diffuse,
	specAlb,
	specPow,
	texObj,
	mvMat,
	projMat,
	lightPos
};

class GraphicsObject{
private:
	vector<GLuint> indices, normIndices, texIndices;
	Material material;
	GLuint VAO, TBO, VBO;
	GLubyte* tgaBits;
	string textureFile;

	GLuint activePowerup;
	const char* tag;
	bool alive;
	GLuint numIndices;
	GLboolean active;

public:
	vector<GLfloat> vertices, normals, texVertices;
	quat rotationQuat;
	vec3 translateVector;
	vec3 center;
	GLuint aiIndex;

	GraphicsObject();
	GraphicsObject(obj *model);
	GraphicsObject(obj *model, string texFile, Material m, const char* tag);
	~GraphicsObject();

	GLboolean isActive(){ return active; }
	void setActive(GLboolean flag){ active = flag; }
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	GLuint getNumIndices();
	void setNumIndices();
	GLuint bufferSize();
	vec3 findMax();
	vec3 findMin();
	void findCenter();

	void bindBuffer(GLboolean inRenderer);
	void genBuffer();
	GLboolean loadTexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode);
	GLboolean readTGABits(GLuint &tWidth, GLuint &tHeight, GLuint &tComponents, GLenum &eFormat);
	void update(vec3 position, quat rotation, int newType);
	void update(vec3 position, quat rotation, int newType, int pLevel);
	void draw(vec3 ambient, vec3 transVec, vec3 scaleVec, mat4 modelView, GLuint *shaderIDs);
	void draw(mat4 modelView, GLuint *shaderIDs);
	void draw(mat4 modelView, GLuint *shaderIDs, vec3 translate, quat rotation);

	GLuint getActivePow();
	void setActivePow(GLuint pow);
	void setTag(const char* newTag){ tag = newTag;}
	const char* getTag(){ return tag;}


	void clear();
	void rearrangeData();
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