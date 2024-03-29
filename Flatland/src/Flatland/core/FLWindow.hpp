#pragma once
#include "GLFW/include/glfw3.h"
#include "GLFW/include/glfw3native.h"
#include "FLInputManager.hpp"


class FLWindow {
public:
	FLWindow(const char* _windowName, uint32_t _width, uint32_t _height);
	~FLWindow();

	GLFWwindow* getWindowPointer() { return window; }
	uint32_t getWidth() const { return width; }
	uint32_t getHeight() const { return height; }

	bool isResized() { return framebufferResized; }
	void resetResizeFlag() { framebufferResized = false; }
private:
	GLFWwindow* window;
	uint32_t width, height;
	const char* windowName;

	bool framebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};