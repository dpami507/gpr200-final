#include "Perlin.h"

//David Amidon
namespace shaderz
{
	static int permutation[256] = { 151,160,137,91,90,15,
		   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	int p(int i)
	{
		return permutation[i & 255];
	}

	float Noise::perlin(float x, float y)
	{
		//Take in a position and create cell coords / mapp to 0-255
		int xi = (int)(floor(x)) & 255;
		int yi = (int)(floor(y)) & 255;

		//Offset coords
		float xf = x - floor(x);
		float yf = y - floor(y);

		//Get 4 corners of the points in hash form
		int ab = p(p(xi) + yi + 1);		//ab---bb
		int aa = p(p(xi) + yi);			//|		 |
		int ba = p(p(xi + 1) + yi);		//|		 |
		int bb = p(p(xi + 1) + yi + 1);	//aa---ba

		//create gradient dot values
		float d1 = grad(aa, xf, yf);
		float d2 = grad(ba, xf - 1.0f, yf);
		float d3 = grad(ab, xf, yf - 1.0f);
		float d4 = grad(bb, xf - 1.0f, yf - 1.0f);

		//Get fade with decimal range
		float u = fade(xf);
		float v = fade(yf);

		//Lerp between gradient vectors
		float x1 = lerp(d1, d2, u);
		float x2 = lerp(d3, d4, u);

		//Lerp between gradient vectors
		float yInter = lerp(x1, x2, v);

		return (yInter + 1.0f) * 0.5f; //0-1
	}

	float Noise::octavePerlin(float x, float y)
	{
		float total = 0;
		float persistence = this->persistence;
		float freq = frequency;
		float amp = this->amplitude;
		float maxValue = 0;

		for (int i = 0; i < octaves; i++)
		{
			total += perlin(x * freq, y * freq) * amp;

			maxValue += amp;

			amp *= persistence;
			freq *= 2.0f;
		}

		float noiseValue = total / maxValue;

		float regionNoise = perlin(x * regionSize, y * regionSize);
		float reuslt = regionNoise * regionInfluence + noiseValue * (1.0 - regionInfluence);

		return reuslt;
	}

	//Generate gradient dot values
	float Noise::grad(int hash, float x, float y)
	{
		switch (hash & 3)
		{
		case 0: return  x + y;
		case 1: return  -x + y;
		case 2: return  x - y;
		case 3: return  -x - y;
		default: return 0;
		}
	}

	//Curve function created by Ken Perlin
	float Noise::fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	//Lerp function between two points
	float Noise::lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	Noise::Noise(int octaves, float frequency)
	{
		std::cout << "Creating Custom Noise Object\n";
		this->octaves = octaves;
		this->frequency = frequency;
	}
	Noise::Noise()
	{
		std::cout << "Creating Default Noise Object\n";
		this->octaves = 4;
		this->frequency = 1;
	}
	Noise::~Noise()
	{
		std::cout << "Deleting Noise Object\n";
	}
}