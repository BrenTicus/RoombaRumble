#include "PowerupObject.h"
#include <cassert>

class Skybox{
private:
	GLuint VAO, VBO, TBO;
	GLchar* vertexShaderFile[1];
	GLchar* fragmentShaderFile[1];
	GLuint shaderProgram;
	GLuint vertShaderPtr, fragShaderPtr;

public:
	Skybox();
	~Skybox();

	bool readShader(const char* filename, int shaderType);
	int setupShaders();
	void bindBuffers();
	void createCubeMap (
		const char* front,
		const char* back,
		const char* top,
		const char* bottom,
		const char* left,
		const char* right,
		GLuint* tex_cube); 
	GLubyte* readTGABits(const char* fileName, GLint &tWidth, GLint &tHeight, GLuint &texComponents, GLenum &format);
	bool loadCubeMapSide(GLuint texture, GLenum side_target, const char* file_name);
	void renderSkybox(glm::mat4 proj, glm::mat4 mv);
};