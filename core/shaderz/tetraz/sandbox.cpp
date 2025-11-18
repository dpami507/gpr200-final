#include "sandbox.h"

glm::vec3 shaderz::raycast_mouse(Camera* cam, int mouse_x, int mouse_y) {
	// normalize each to a range of [-1, 1]
	float xCoord = (2 * mouse_x) / shaderz::SCREEN_WIDTH - 1.0f;
	float yCoord = 1.0f - (2.0f * mouse_y) / shaderz::SCREEN_HEIGHT;
	float zCoord = 1.0f;
	// make the normal ray
	glm::vec3 normalRay = glm::vec3(xCoord, yCoord, zCoord);

	// we want the ray's z to point forewards
	glm::vec4 rayClip = glm::vec4(normalRay.x, normalRay.y, -1.0, 1.0);
	// get camera coordinates and apply them
	glm::vec4 rayWithCam = glm::inverse(cam->getProjection()) * rayClip;
	// manually set z and w values to be forward for the ray
	rayWithCam = glm::vec4(rayWithCam.x, rayWithCam.y, -1.0, 0.0);

	// set the ray to world coordinates
	glm::vec3 rayToWorld = glm::vec3((glm::inverse(cam->getView()) * rayWithCam).x, (glm::inverse(cam->getView()) * rayWithCam).y, (glm::inverse(cam->getView()) * rayWithCam).z);
	// normalize the final ray and send it back
	return glm::normalize(rayToWorld);
}
