#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GLFW/glfw3.h>

namespace shaderz {
	class Time
	{
	public:
		static float time;
		static float deltaTime;
		static float timeScale;
		static float fps;

		static void Update()
		{
			time = glfwGetTime() * timeScale;
			deltaTime = time - lastTime;
			lastTime = time;
			fps = round(1.0 / deltaTime);
		}

	private:
		static float lastTime;
	};

	float Time::time = 0.0f;
	float Time::deltaTime = 0.0f;
	float Time::lastTime = 0.0f;
	float Time::timeScale = 1.0f;
	float Time::fps = 0.0f;
}