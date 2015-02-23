#include "GraphicsObject.h"

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