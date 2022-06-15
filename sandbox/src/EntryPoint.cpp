#include "Flatland/core/EntryPoint.hpp"
#include "game.hpp"

bool createGame(gameInstance* gameInst) {
	gameInst->app_config.name = "Flatland sandbox";
	gameInst->app_config.width = 1280;
	gameInst->app_config.height = 720;
	gameInst->initialize = game_initialize;
	gameInst->resize = game_resize;
	gameInst->update = game_update;
	gameInst->render = game_render;

	return true;
}