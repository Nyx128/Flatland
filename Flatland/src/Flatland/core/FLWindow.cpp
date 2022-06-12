#include "FLWindow.hpp"
#include "logger.hpp"
#include "asserts.hpp"

FLWindow::FLWindow(const char* _windowName, uint32_t _width, uint32_t _height): width(_width), height(_height), windowName(_windowName) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	FL_INFO("window set to non-resizable");

	window = glfwCreateWindow(width, height, windowName, NULL, NULL);
	FL_INFO("Window created (width: %d, height: %d)", width, height);

	FL_ASSERT_MSG(window != 0, "Failed to create window");
}

FLWindow::~FLWindow(){
	glfwDestroyWindow(window);
	FL_INFO("Window destroyed");
	glfwTerminate();
}

