#pragma once
#include "component.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif 

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED
#endif 

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1280
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 720
#endif

struct Transform2D {

	glm::vec2 scale = glm::vec2(1.0f, 1.0f);
	glm::vec2 position = glm::vec2(0.0f, 0.0f);
	float rotation = 0.0f;

	glm::mat2& getTransformMatrix() {
		glm::mat2 scale_matrix = glm::mat2(1.0f);
		glm::mat2 rot_matrix = glm::mat2(1.0f);
						  //1st comlumn//  //2nd column//
		scale_matrix = { {scale.x, 0.0f}, {0.0f, scale.y} };

		float c = glm::cos(rotation);
		float s = glm::sin(rotation);
		rot_matrix = { {c, s}, {-s, c} };
		float aspect_ratio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
		float half_width = 2.0f;
		glm::mat2 ortho = glm::ortho(-half_width * aspect_ratio , half_width * aspect_ratio, -half_width, half_width);
		glm::mat2 result = ortho * rot_matrix * scale_matrix;

		return result;
	}
};
