#pragma once

#include "../Object.h"
#include "../Camera.h"

namespace shaderz {
	const int SCREEN_WIDTH = 1000;
	const int SCREEN_HEIGHT = 1000;

	glm::vec3 raycastMouse(Camera* cam, int mouse_x, int mouse_y);
	glm::vec3 rayCollision(Object* obj, Camera* cam, int mouse_x, int mouse_y);
}