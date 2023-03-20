#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <math.h>

struct Rect {
	float x, y, w, h;
	int pos;
};

std::vector<float> VERTICES;
std::vector<unsigned int> INDICES;
std::vector<Rect> RECTS;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void handleEvents(GLFWwindow* window);
void onMouseClicked(GLFWwindow* window, int button, int action, int mods);
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 720;
Rect currentlyCreatedRect{};
GLFWwindow* WINDOW;
const double STEP = 0.05;

bool isDragging = false;

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

	WINDOW = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rectangle packing problem", NULL, NULL);
	if (WINDOW == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(WINDOW);
	glfwSetFramebufferSizeCallback(WINDOW, framebuffer_size_callback);
	glfwSetMouseButtonCallback(WINDOW, onMouseClicked);

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

void addRect(Rect* rect, std::vector<float>* vertices, std::vector<unsigned int>* indices) {
	auto lengthBefore = vertices->size() / 3;
	vertices->push_back(rect->x + rect->w); //TR
	vertices->push_back(rect->y);
	vertices->push_back(0.0f);

	vertices->push_back((rect->x + rect->w)); //BR
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

void createVertexArray(unsigned int* VBO, unsigned int* VAO, std::vector<float>* vertices, std::vector<unsigned int>* indices) {

	unsigned int EBO;
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, VBO);
	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices->size(), vertices->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(), indices->data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

int main()
{
	if (!init() || !buildShaders()) {
		return -1;
	}

	unsigned int VBO, VAO;

	while (!glfwWindowShouldClose(WINDOW))
	{
		handleEvents(WINDOW);

		VERTICES.clear();
		INDICES.clear();

		addRect(&currentlyCreatedRect, &VERTICES, &INDICES);
		for (int i = 0; i < RECTS.size(); i++) {
			Rect r = RECTS[i];
			addRect(&r, &VERTICES, &INDICES);
		}

		createVertexArray(&VBO, &VAO, &VERTICES, &INDICES);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render(VAO, VERTICES.size());

		glfwSwapBuffers(WINDOW);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(SHADER_PROGRAM);

	glfwTerminate();
	return 0;
}

void convertToOpenglCoordSystem(double x, double y, double* ox, double* oy) {
	*ox = 2 * x / SCR_WIDTH - 1;
	*oy = -2 * y / SCR_HEIGHT + 1;
}

void setWidthAndHeight(float TL_x, float TL_y, float BR_x, float BR_y, Rect* rect) {

	float left = std::min(TL_x, BR_x);
	float right = std::max(TL_x, BR_x);
	float top = std::min(TL_y, BR_y);
	float bottom = std::max(TL_y, BR_y);

	float width = right - left;
	float height = bottom - top;

	rect->w = width;
	rect->h = height;
}

void moveToClosestStep(double step, double* x, double* y) {
	*x = std::floor(*x / step) * step;
	*y = std::floor(*y / step) * step;

	printf("%f %f\r", *x, *y);
}

void handleEvents(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (isDragging) {
		double xpos, ypos, xop, yop;
		glfwGetCursorPos(WINDOW, &xpos, &ypos);
		convertToOpenglCoordSystem(xpos, ypos, &xop, &yop);

		moveToClosestStep(STEP, &xop, &yop);

		setWidthAndHeight(currentlyCreatedRect.x, currentlyCreatedRect.y, xop, yop, &currentlyCreatedRect);
	}
}

bool isPointInRect(Rect* r, float x, float y) {
	float max_x = r->x + r->w;
	float min_x = r->x;

	float max_y = r->y;
	float min_y = r->y - r->h;

	return x <= max_x && x >= min_x && y >= min_y && y <= max_y;
}

bool isOverlapping(Rect* rect, std::vector<Rect> rects) {
	float rect_x1 = rect->x;
	float rect_x2 = rect->x + rect->w;
	float rect_y1 = rect->y - rect->h;
	float rect_y2 = rect->y;

	for (auto& r : rects) {
		float r_x1 = r.x;
		float r_x2 = r.x + r.w;
		float r_y1 = r.y - r.h;
		float r_y2 = r.y;

		if(rect_x1 < r_x2 && rect_x2 > r_x1 &&
		   rect_y1 < r_y2 && rect_y2 > r_y1)
			return true;
	}

	return false;
}

void onMouseClicked(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos, xop, yop;
	glfwGetCursorPos(WINDOW, &xpos, &ypos);
	convertToOpenglCoordSystem(xpos, ypos, &xop, &yop);

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		for (int i = RECTS.size() - 1; i >= 0; i--) {
			if (isPointInRect(&RECTS[i], xop, yop)) {
				RECTS.erase(RECTS.begin() + i);
				break; //Note(Igor): We break since we want to remove only the "top" rectangle
			}
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		currentlyCreatedRect.x = xop;
		currentlyCreatedRect.y = yop;
		isDragging = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isDragging = false;

		if (!isOverlapping(&currentlyCreatedRect, RECTS)) {
			RECTS.push_back(currentlyCreatedRect);
		}
		currentlyCreatedRect = {};
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}