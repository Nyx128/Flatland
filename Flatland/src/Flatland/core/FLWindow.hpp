#pragma once
#include "../core.hpp"
#include "GLFW/include/glfw3.h"

class FLWindow {
public:
	FLWindow(const char* _windowName, uint32_t _width, uint32_t _height);
	~FLWindow();

	GLFWwindow* getWindowPointer() const { return window; }
private:
	GLFWwindow* window;
	uint32_t width, height;
	const char* windowName;
};