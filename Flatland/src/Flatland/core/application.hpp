#pragma once
#include "../core.hpp"
#include "FLWindow.hpp"
#include "gameInstance.hpp"
#include <memory>

class FL_API Application{
public:
	Application(gameInstance* _gameInst);
	~Application();

	void run();

private:
	std::unique_ptr<FLWindow> window;
	gameInstance* gameInst;
};
