#include "game.hpp"

bool game_initialize(gameInstance* gameInst){
    FL_DEBUG("game init() called");
    return true;
}

bool game_update(gameInstance* gameInst, float deltaTime)
{
    return true;
}

bool game_render(gameInstance* gameInst, float deltaTime)
{
    return true;
}

void game_resize(gameInstance* gameInst, uint32_t width, uint32_t height){
}
