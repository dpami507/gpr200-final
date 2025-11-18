#ifndef TERRAIN_H
#define TERRAIN_H

#include "Object.h"

//David Amidon
namespace shaderz {
	class Terrain : public Object
	{
	public:
		Terrain(const FastNoiseLite& noise, float size, int segments);
		void GenerateNoiseTexture(float size, int segments);
		void draw(bool drawAsPoints = false, bool drawWireframe = false);
		void load(float size, int segments, FastNoiseLite noise);
	private:
		GLuint terrainTextureID;
		unsigned char* data;
		FastNoiseLite noise;
	};
}

#endif