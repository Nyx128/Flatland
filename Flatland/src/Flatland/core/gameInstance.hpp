#pragma once
#include <stdint.h>

struct ApplicationConfig {
	uint32_t width;
	uint32_t height;
	const char* name;
};

typedef struct gameInstance {
	ApplicationConfig app_config;

	bool (*initialize)(gameInstance* gameInstance);
	bool (*update)(gameInstance* gameInstance, float deltaTime);
	bool (*render)(gameInstance* gameInstance, float deltaTime);
	void (*resize)(gameInstance* gameInstance, uint32_t width, uint32_t height);
} gameInstance;