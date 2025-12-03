#ifndef TERRAIN_H
#define TERRAIN_H

#include "Object.h"
#include "Texture2D.h"
#include "Perlin.h"

//David Amidon
namespace shaderz {
	class Terrain : public Object
	{
	public:
		Terrain(const Noise& noise, float size, int segments);
		~Terrain();
		void GenerateNoiseTexture(float size, int segments);

		void BindTerrainTexture(int slot);
		void BindNoiseTexture(int slot);

		void draw(bool drawAsPoints = false, bool drawWireframe = false);
		void load(float size, int segments, Noise noise);
	private:
		int segments;
		float* noiseData;
		Noise noise;
		GLuint noiseID;
	};
}

#endif