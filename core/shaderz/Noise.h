#ifndef NOISE_H
#define NOISE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"

namespace shaderz {
	class Noise
	{
	public:
		//Perlin Noise (https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)
		float GenNoise(glm::vec2 position, float frequency);
		float PerlinNoise(glm::vec2 position, int octaveCount);
		std::vector<std::vector<float>> GeneratePerlinMap(int size, int segments, float noiseScale, int octaveCount)
		{
			std::vector<std::vector<float>> map;
			map.reserve(segments + 1);

			for (size_t x = 0; x <= segments; x++)
			{
				float xPos = ((float)x / segments) * size;
				std::vector<float> yMap;

				for (size_t y = 0; y <= segments; y++)
				{
					float yPos = ((float)y / segments) * size;
					float height = PerlinNoise(glm::vec2(xPos, yPos) * noiseScale, octaveCount);
					yMap.push_back(height);
				}

				map.push_back(yMap);
			}

			return map;
		}
	private:
		float rand(glm::vec2 coord) {
			glm::vec2 k1 = glm::vec2(
				23.14069263277926f, // e^pi (Gelfond's constant)
				2.665144142690225f  // 2^sqrt(2) (Gelfond-Schneider constant)
			);
			return frac(cos(glm::dot(coord, k1)) * 12345.6789f);
		}
		float frac(float x) { return x - floor(x); }
		float mix(float x, float y, float a) { return a * (1 - a) + y * a; }
	};
}

#endif