#include "FLInputManager.hpp"
#include <stdio.h>

FLInputManager::FLInputManager(){

}

FLInputManager::~FLInputManager()
{
}

void FLInputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (keyStates.find(key) == keyStates.end()) {
		if (action != GLFW_RELEASE) {
			keyStates.insert(std::make_pair(key, FLInputState::FL_PRESSED));
		}
		else {
			keyStates.insert(std::make_pair(key, FLInputState::FL_RELEASED));
		}
	}
	else {
		if (action != GLFW_RELEASE) {
			keyStates[key] = FLInputState::FL_PRESSED;
		}
		else {
			keyStates[key] = FLInputState::FL_RELEASED;
		}
	}
}

void FLInputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (mouseButtonStates.find(button) == mouseButtonStates.end()) {
		if (action != GLFW_RELEASE) {
			mouseButtonStates.insert(std::make_pair(button, FLInputState::FL_PRESSED));
		}
		else {
			mouseButtonStates.insert(std::make_pair(button, FLInputState::FL_RELEASED));
		}
	}
	else {
		if (action != GLFW_RELEASE) {
			mouseButtonStates[button] = FLInputState::FL_PRESSED;
		}
		else {
			mouseButtonStates[button] = FLInputState::FL_RELEASED;
		}
	}
}

bool FLInputManager::isKeyPressed(int keycode){
	if (keyStates.find(keycode) == keyStates.end()) {
		keyStates.insert(std::make_pair(keycode, FLInputState::FL_RELEASED));
		return false;
	}
	else {
		return (keyStates[keycode] == FLInputState::FL_PRESSED) ? true : false;
	}
}

bool FLInputManager::isMouseButtonPressed(int buttonCode){
	if (mouseButtonStates.find(buttonCode) == mouseButtonStates.end()) {
		mouseButtonStates.insert(std::make_pair(buttonCode, FLInputState::FL_RELEASED));
		return false;
	}
	else {
		return (mouseButtonStates[buttonCode] == FLInputState::FL_PRESSED) ? true : false;
	}
}
