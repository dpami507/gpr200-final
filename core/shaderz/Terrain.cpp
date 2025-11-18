#include "Terrain.h"

namespace shaderz {
    void Terrain::GenerateNoiseTexture(float size, int segments)
    {
        std::cout << "Creating Terrain Texture\n";

        int numberofPoints = (segments + 1) * (segments + 1);

        data = new unsigned char[numberofPoints];

        float sampleOffset = (float)size / segments;
        int maxIndex = -1;  // Track the maximum index we try to access

        for (size_t row = 0; row <= segments; row++)
        {
            float zPos = size * ((float)row / segments) - (size / 2);
            for (size_t col = 0; col <= segments; col++)
            {
                float xPos = size * ((float)col / segments) - (size / 2);
                int index = (row * (segments + 1)) + col;

                if (index > maxIndex) maxIndex = index;

                float noiseValue = noise.GetNoise(xPos + sampleOffset, zPos);
                data[index] = (unsigned char)((noiseValue + 1.0f) * 127.5f);
            }
        }

        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, segments + 1, segments + 1, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

	Terrain::Terrain(const FastNoiseLite& noise, float size, int segments) : Object(*mesh)
	{
		this->noise = noise;

        GenerateNoiseTexture(size, segments);
		MeshData terrainData = createTerrain(size, segments, data);
		this->mesh = new Mesh(terrainData);

		std::cout << "Created Terrain Object\n";
	}

	void Terrain::draw(bool drawAsPoints, bool drawWireframe)
	{
		mesh->draw(drawAsPoints, drawWireframe);
	}

	//Reloads MeshData to update values
	void Terrain::load(float size, int segments, FastNoiseLite noise)
	{
        this->noise = noise;
        GenerateNoiseTexture(size, segments);
		MeshData terrainData = createTerrain(size, segments, data);
		mesh->load(terrainData);
	}
}