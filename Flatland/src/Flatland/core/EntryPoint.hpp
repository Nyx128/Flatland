#pragma once
#include "logger.hpp"
#include "application.hpp"
#include "gameInstance.hpp"

extern bool createGame(gameInstance* gameInst);

int main() {
	gameInstance gameInst;
	if (!createGame(&gameInst)) {
		FL_FATAL("Failed to create game");
	}

	//check for game's function pointers.
	if (!gameInst.initialize || !gameInst.update || !gameInst.render) {
		FL_FATAL("The game's function pointers must be assigned");
		return -2;
	}

	Application application(&gameInst);

	application.run();
}
