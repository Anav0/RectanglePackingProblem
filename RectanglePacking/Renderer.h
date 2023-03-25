#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Misc.h"

#include <vector>

class Renderer
{
private:
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";
	
public:
	unsigned int SHADER_PROGRAM;
	GLFWwindow*  WINDOW;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	void CreateVertexArray(unsigned int* VBO, unsigned int* VAO);
	void Render(unsigned int VAO);
	bool BuildShaders();
};
