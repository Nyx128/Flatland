#include "FLWindow.hpp"
#include "logger.hpp"

FLWindow::FLWindow(const char* _windowName, uint32_t _width, uint32_t _height): width(_width), height(_height), windowName(_windowName) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName, NULL, NULL);
	FL_INFO("Window created (width: %d, height: %d)", width, height);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, FLInputManager::key_callback);
	glfwSetMouseButtonCallback(window, FLInputManager::mouse_button_callback);
}

FLWindow::~FLWindow(){
	glfwDestroyWindow(window);
	FL_INFO("Window destroyed");
	glfwTerminate();
}

void FLWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height){
	auto flWindow = reinterpret_cast<FLWindow*>(glfwGetWindowUserPointer(window));
	flWindow->framebufferResized = true;
	flWindow->width = width;
	flWindow->height = height;
}

