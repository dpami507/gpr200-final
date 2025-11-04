#ifndef TIME_H
#define TIME_H

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

		static void Update();

	private:
		static float lastTime;
	};
}

#endif