#ifndef PERLIN_H
#define PERLIN_H

#include <iostream>
#include "Time.h"

//Created following this tutorial: https://www.youtube.com/watch?v=iW4nFygKAjw and https://adrianb.io/2014/08/09/perlinnoise.html
namespace shaderz
{
	class Noise
	{
	public:
		Noise();
		Noise(int octaves, float frequency);
		~Noise();

		float grad(int hash, float x, float y);
		float fade(float t);
		float lerp(float a, float b, float t);

		float perlin(float x, float y);
		float octavePerlin(float x, float y);
		void SetOctaves(int octaves) { this->octaves = octaves; }
		void SetFrequency(float frequency) { this->frequency = frequency; }

		int GetFrequency() const { return frequency; }
		int GetOctaves() const { return octaves; }

	private:
		int octaves = 4;
		float frequency = 0.5f;
	};
}

#endif