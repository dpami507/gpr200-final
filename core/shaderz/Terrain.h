#ifndef TERRAIN_H
#define TERRAIN_H

#include "Object.h"
#include "Texture2D.h"

//David Amidon
namespace shaderz {
	class Terrain : public Object
	{
	public:
		Terrain(const FastNoiseLite& noise, float size, int segments);
		void GenerateNoiseTexture(float size, int segments, std::vector<std::pair<Texture2D*, float>> textures = { {nullptr, 0.0}, {nullptr, 0.0} });

		void BindTerrainTexture(int slot);
		void BindNoiseTexture(int slot);

		void draw(bool drawAsPoints = false, bool drawWireframe = false);
		void load(float size, int segments, FastNoiseLite noise);
		GLuint noiseID;
		GLuint terrainTextureID;
	private:
		int segments;
		float* noiseData;
		float* texData;
		FastNoiseLite noise;
	};
}

#endif