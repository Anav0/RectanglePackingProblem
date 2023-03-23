#pragma once
#include <GLFW/glfw3.h>

#include "Misc.h"

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

enum MouseButton {
	NONE,
	LEFT,
	RIGHT
};

enum MouseAction {
	NEITHER,
	PRESSED,
	RELEASED
};

class GlfwWindow
{
public:
	GLFWwindow* WINDOW;
	int SCR_WIDTH = 1920, SCR_HEIGHT = 1080;

	double mouse_x, mouse_y;
	MouseButton buttonType;
	MouseAction buttonAction;

	bool Init();
	bool IsClosing();
	void onMouseClicked(GLFWwindow* window, int button, int action, int mods);
	void onBeginOfTheLoop();
};