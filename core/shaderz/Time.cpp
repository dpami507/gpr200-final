#include "Time.h"

namespace shaderz {
	float Time::time = 0.0f;
	float Time::deltaTime = 0.0f;
	float Time::lastTime = 0.0f;
	float Time::timeScale = 1.0f;
	float Time::fps = 0.0f;

	void Time::Update()
	{
		time = glfwGetTime() * timeScale;
		deltaTime = time - lastTime;
		lastTime = time;
		fps = round(1.0 / deltaTime);
	}
}