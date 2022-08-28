#pragma once
#include "GLFW/include/glfw3.h"
#include <unordered_map>

//defines
#define FL_KEY_UP GLFW_KEY_UP
#define FL_KEY_DOWN GLFW_KEY_DOWN
#define FL_KEY_LEFT GLFW_KEY_LEFT
#define FL_KEY_RIGHT GLFW_KEY_RIGHT
#define FL_KEY_SPACE GLFW_KEY_SPACE

#define FL_MOUSE_LEFT GLFW_MOUSE_BUTTON_LEFT 
#define FL_MOUSE_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE
#define FL_MOUSE_RIGHT GLFW_MOUSE_BUTTON_RIGHT
//

class FLInputManager {
public:

	FLInputManager();
	~FLInputManager();


	enum class FLInputState {
		FL_RELEASED = 0,
		FL_PRESSED = 1
	};

	static bool isKeyPressed(int keycode);
	static bool isMouseButtonPressed(int buttonCode);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
private:
	inline static std::unordered_map<int, FLInputState> keyStates;
	inline static std::unordered_map<int, FLInputState> mouseButtonStates;
};
