#include "Terrain.h"

namespace shaderz {
    void Terrain::GenerateNoiseTexture(float size, int segments)
    {
        std::cout << "Creating Noise/Terrain Texture\n";

        if (segments != this->segments)
            this->segments = segments;

        int numberofPoints = (segments + 1) * (segments + 1);
        noiseData = new float[numberofPoints];
        texData = new float[numberofPoints * 3];

        float sampleOffset = (float)size / segments;
        float halfSize = size / 2.0f;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &noiseID);
        glBindTexture(GL_TEXTURE_2D, noiseID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generate Data
        for (size_t row = 0; row <= segments; row++)
        {
            float zPos = row * sampleOffset - halfSize;

            for (size_t col = 0; col <= segments; col++)
            {
                //Noise Data
                float xPos = col * sampleOffset - halfSize;
                int index = ((segments - row) * (segments + 1)) + col;
                float noiseValue = (noise.GetNoise(xPos, zPos) + 1.0f) / 2.0f;

                noiseData[index] = noiseValue;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segments + 1, segments + 1, 0, GL_RED, GL_FLOAT, noiseData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //Generate Data
        for (size_t row = 0; row <= segments; row++)
        {
            float zPos = row * sampleOffset - halfSize;

            for (size_t col = 0; col <= segments; col++)
            {
                //Texture Data
                float xPos = col * sampleOffset - halfSize;
                int index = (((segments - row) * (segments + 1)) + col) * 3;
                float noiseValue = (noise.GetNoise(xPos, zPos) + 1.0f) / 2.0f;

                glm::vec3 color = glm::vec3(noiseValue);

                if (textures.size() > 0)
                {
                    for (auto tex : textures)
                    {
                        if (noiseValue < tex.second)
                        {
                            color = tex.first->Sample(glm::vec2((float)col / segments, (float)row / segments));
                            break;
						}
                    }
     //               if (noiseValue > 0.6)
     //                   color = glm::vec3(0.5f, 0.5f, 0.5f); // Rock
     //               else if (noiseValue > 0.5)
     //                   color = glm::vec3(0.0f, 1.0f, 0.0f); // Grass
     //               else if (noiseValue > 0.45)
     //                   color = glm::vec3(0.76f, 0.70f, 0.50f); // Sand
					//else color = glm::vec3(0.8f, 0.2f, 0.2f); // Dirt
                }


                texData[index] = color.r;
                texData[index + 1] = color.g;
                texData[index + 2] = color.b;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, segments + 1, segments + 1, 0, GL_RGB, GL_FLOAT, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Terrain::BindNoiseTexture(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, noiseID);
    }

    void Terrain::BindTerrainTexture(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);
    }

	Terrain::Terrain(const FastNoiseLite& noise, float size, int segments, std::vector<std::pair<Texture2D*, float>> textures) : Object(*mesh)
	{
		this->noise = noise;
        this->segments = segments;

		this->textures = textures;

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