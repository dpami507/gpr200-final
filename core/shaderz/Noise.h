#ifndef NOISE_H
#define NOISE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"

namespace shaderz {
	class Noise
	{
	public:
		//Perlin Noise (https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)
		float GenNoise(glm::vec2 p, float frequency);
		float PerlinNoise(glm::vec2 p, int res);
	private:
		float rand(glm::vec2 c) {
			float x = sin(dot(glm::vec2(c.x, c.y), glm::vec2(12.9898, 78.233))) * 43758.5453;
			return x - floor(x);
		}
		float mix(float x, float y, float a) { return a * (1 - a) + y * a; }
	};
}

#endif