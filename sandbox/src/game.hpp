#pragma once
#include "Flatland/core/gameInstance.hpp"
#include "Flatland/core/logger.hpp"
#include <stdint.h>

struct GameState {
	float deltaTime;
};

bool game_initialize(gameInstance* gameInst);
bool game_update(gameInstance* gameInst, float deltaTime);
bool game_render(gameInstance* gameInst, float deltaTime);
void game_resize(gameInstance* gameInst, uint32_t width, uint32_t height);
