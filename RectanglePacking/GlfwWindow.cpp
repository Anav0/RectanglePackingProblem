#include <glad/glad.h>
#include "GlfwWindow.h"

#include <iostream>

void GlfwWindow::onBeginOfTheLoop() {
	double xpos, ypos;
	glfwGetCursorPos(WINDOW, &xpos, &ypos);
	convertToOpenglCoordSystem(xpos, ypos, &mouse_x, &mouse_y, SCR_WIDTH, SCR_HEIGHT);

	if (glfwGetKey(WINDOW, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(WINDOW, true);

	buttonType   = NONE;
	buttonAction = NEITHER;
}

void GlfwWindow::onMouseClicked(GLFWwindow* window, int button, int action, int mods) {
	
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		buttonType = RIGHT;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
		buttonType = LEFT;

	if (action == GLFW_PRESS)
		buttonAction = PRESSED;

	if (action == GLFW_RELEASE)
		buttonAction = RELEASED;
}

bool GlfwWindow::Init() {
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

	glfwSetWindowUserPointer(WINDOW, this);

	auto func = [](GLFWwindow* w, int btn, int action, int mods)
	{
		static_cast<GlfwWindow*>(glfwGetWindowUserPointer(w))->onMouseClicked(w, btn, action, mods);
	};
	glfwSetMouseButtonCallback(WINDOW, func);

	glfwMakeContextCurrent(WINDOW);
	glfwSetFramebufferSizeCallback(WINDOW, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	return true;
}

bool GlfwWindow::IsClosing()
{
	return glfwWindowShouldClose(WINDOW);
}
