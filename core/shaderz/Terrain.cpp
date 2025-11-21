#include "Terrain.h"

namespace shaderz {
    void Terrain::GenerateNoiseTexture(float size, int segments)
    {
        std::cout << "Creating Terrain Texture\n";

        if (segments != this->segments)
            this->segments = segments;

        int numberofPoints = (segments + 1) * (segments + 1);

        noiseData = new float[numberofPoints];

        float sampleOffset = (float)size / segments;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generate Data
        for (size_t row = 0; row <= segments; row++)
        {
            float zPos = size * ((float)row / segments) - (size / 2);
            for (size_t col = 0; col <= segments; col++)
            {
                float xPos = size * ((float)col / segments) - (size / 2);
                int index = (row * (segments + 1)) + col;

                float noiseValue = (noise.GetNoise(xPos + sampleOffset, zPos) + 1.0f) / 2;

                noiseData[index] = noiseValue;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segments + 1, segments + 1, 0, GL_RED, GL_FLOAT, noiseData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Terrain::BindTerrainTexture(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);
    }

	Terrain::Terrain(const FastNoiseLite& noise, float size, int segments) : Object(*mesh)
	{
		this->noise = noise;
        this->segments = segments;

        GenerateNoiseTexture(size, segments);
		MeshData terrainData = createTerrain(size, segments, noiseData);
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
		MeshData terrainData = createTerrain(size, segments, noiseData);
		mesh->load(terrainData);
	}
}