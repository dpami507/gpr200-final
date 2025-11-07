#include "Noise.h"

namespace shaderz {
	float Noise::GenNoise(glm::vec2 p, float frequency)
	{
		float unit = 1 / frequency;
		glm::vec2 ij = floor(p / unit);
		glm::vec2 xy = mod(p, unit) / unit;
		xy = (glm::vec2(1.0) - cos(ew::PI * xy)) * (float)0.5;
		float a = rand((ij + glm::vec2(0., 0.)));
		float b = rand((ij + glm::vec2(1., 0.)));
		float c = rand((ij + glm::vec2(0., 1.)));
		float d = rand((ij + glm::vec2(1., 1.)));
		float x1 = mix(a, b, xy.x);
		float x2 = mix(c, d, xy.x);
		return mix(x1, x2, xy.y);
	}

	float Noise::PerlinNoise(glm::vec2 p, int res) {
		float persistance = 0.5;
		float n = 0.0;
		float normK = 0.0;
		float f = 4.0;
		float amp = 1.0;
		int iCount = 0;
		for (int i = 0; i < 50; i++) {
			n += amp * GenNoise(p, f);
			f *= 2.;
			normK += amp;
			amp *= persistance;
			if (iCount == res) break;
			iCount++;
		}
		float nf = n / normK;
		return nf * nf * nf * nf;
	}
}