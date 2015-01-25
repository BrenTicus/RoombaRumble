#include "Renderer.h"

Renderer::Renderer()
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
	}
}


Renderer::~Renderer()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::Update()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Note that buffer swapping and polling for events is done here so please don't do it in the function used to draw the scene.
	glfwSwapBuffers(window);
	glfwPollEvents();
}