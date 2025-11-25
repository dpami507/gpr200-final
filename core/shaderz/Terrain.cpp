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

        glGenTextures(1, &noiseID);
        glBindTexture(GL_TEXTURE_2D, noiseID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generate Data
        for (size_t row = 0; row <= segments; row++)
        {
            float zPos = row * sampleOffset - (segments / 2);
            for (size_t col = 0; col <= segments; col++)
            {
                float xPos = col * sampleOffset - (segments / 2);
                int index = (row * (segments + 1)) + col;

                float noiseValue = (noise.GetNoise(xPos, zPos) + 1.0f) / 2.0f;

                noiseData[index] = noiseValue;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segments + 1, segments + 1, 0, GL_RED, GL_FLOAT, noiseData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Terrain::BindNoiseTexture(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, noiseID);
    }

    void Terrain::CreateTerrainTexture(float size, int res, std::vector<std::pair<Texture2D*, float>> textures)
    {
        std::cout << "Creating Custom Texture\n";

        int numberofPoints = res * res * 3;
        texData = new float[numberofPoints];

        float sampleOffset = (float)size / res;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generate Data
        for (size_t row = 0; row < res; row++)
        {
            float zPos = row * sampleOffset - (res / 2);
            for (size_t col = 0; col < res; col++)
            {
                float xPos = col * sampleOffset - (res / 2);

                int index = ((row * res) + col) * 3;
                float noiseValue = (noise.GetNoise(xPos, zPos) + 1.0f) / 2.0f;

                glm::vec3 color = glm::vec3(0.0f);

                if(noiseValue > 0.7)
                    color = glm::vec3(0.5f, 0.5f, 0.5f); //Rock
                else if (noiseValue > 0.3)
                    color = glm::vec3(0.1f, 0.8f, 0.1f); //Grass
                else
					color = glm::vec3(0.1f, 0.1f, 0.8f); //Water

                texData[index] =     color.r;
                texData[index + 1] = color.g;
                texData[index + 2] = color.b;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, res, res, 0, GL_RGB, GL_FLOAT, texData);
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

        std::vector<std::pair<Texture2D*, float>> texs = 
        {
            {nullptr, 0.5},
            {nullptr, 0.5}
        };

        CreateTerrainTexture(size, 512, texs);
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

        std::vector<std::pair<Texture2D*, float>> texs =
        {
            {nullptr, 0.5},
            {nullptr, 0.5}
        };
        CreateTerrainTexture(size, 512, texs);

		MeshData terrainData = createTerrain(size, segments, noiseData);
		mesh->load(terrainData);
	}
}