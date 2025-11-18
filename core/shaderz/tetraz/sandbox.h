#pragma once
#include "../Camera.h"

namespace shaderz {
	glm::vec3 raycast_mouse(Camera* cam, int mouse_x, int mouse_y);
}