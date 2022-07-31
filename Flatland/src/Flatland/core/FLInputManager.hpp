#pragma once
#include "GLFW/include/glfw3.h"
#include <unordered_map>

class FLInputManager {
public:

	FLInputManager();
	~FLInputManager();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	enum class FLInputState {
		FL_RELEASED = 0,
		FL_PRESSED = 1
	};

	static bool isKeyPressed(int keycode);
	static bool isMouseButtonPressed(int buttonCode);
private:
	inline static std::unordered_map<int, FLInputState> keyStates;
	inline static std::unordered_map<int, FLInputState> mouseButtonStates;
};
