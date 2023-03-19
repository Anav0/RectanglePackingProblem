#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 720;
GLFWwindow* WINDOW;

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

unsigned int SHADER_PROGRAM;

bool init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	WINDOW = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (WINDOW == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(WINDOW);
	glfwSetFramebufferSizeCallback(WINDOW, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	return true;
}

bool buildShaders() {
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	// link shaders
	SHADER_PROGRAM = glCreateProgram();
	glAttachShader(SHADER_PROGRAM, vertexShader);
	glAttachShader(SHADER_PROGRAM, fragmentShader);
	glLinkProgram(SHADER_PROGRAM);

	// check for linking errors
	glGetProgramiv(SHADER_PROGRAM, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(SHADER_PROGRAM, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void render(unsigned int VAO, int count) {
	glUseProgram(SHADER_PROGRAM);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

struct Rect {
	float x, y, w, h;
	int pos;
};

void addRect(Rect* rect, std::vector<float>* vertices, std::vector<unsigned int>* indices) {
	auto lengthBefore = vertices->size();
	vertices->push_back(rect->x + rect->w); //TR
	vertices->push_back(rect->y);
	vertices->push_back(0.0f);

	vertices->push_back((rect->x + rect->w) ); //BR
	vertices->push_back(rect->y - rect->h);
	vertices->push_back(0.0f);

	vertices->push_back(rect->x); //BL
	vertices->push_back(rect->y - rect->h);
	vertices->push_back(0.0f);

	vertices->push_back(rect->x); //TL
	vertices->push_back(rect->y);
	vertices->push_back(0.0f);

	indices->push_back(lengthBefore);
	indices->push_back(lengthBefore + 1);
	indices->push_back(lengthBefore + 3);
	indices->push_back(lengthBefore + 1);
	indices->push_back(lengthBefore + 2);
	indices->push_back(lengthBefore + 3);
}

int main()
{
	if (!init()) {
		return -1;
	}

	if (!buildShaders()) {
		return -1;
	}

	std::vector<float> vertices2;
	std::vector<unsigned int> indices2;
	Rect r1{};
	r1.x = -0.05;
	r1.y = 0.5;
	r1.w = 0.5;
	r1.h = 0.5;

	addRect(&r1, &vertices2, &indices2);

	unsigned int EBO, VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices2.size(), indices2.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	while (!glfwWindowShouldClose(WINDOW))
	{
		processInput(WINDOW);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		render(VAO, vertices2.size());

		glfwSwapBuffers(WINDOW);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(SHADER_PROGRAM);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}