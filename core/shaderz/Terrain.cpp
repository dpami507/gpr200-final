#include "Terrain.h"

namespace shaderz {
    void Terrain::GenerateNoiseTexture(float size, int segments)
    {
        std::cout << "Creating Noise/Terrain Texture\n";

        if (segments != this->segments)
            this->segments = segments;

		//Create Noise Texture for Heightmap
        int numberofPoints = (segments + 1) * (segments + 1);
        noiseData = new float[numberofPoints];

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

				float distanceFromCenter = sqrt(xPos * xPos + zPos * zPos) - 0.4;
                //noiseValue -= distanceFromCenter;

                noiseData[index] = noiseValue;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segments + 1, segments + 1, 0, GL_RED, GL_FLOAT, noiseData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glGenTextures(1, &terrainTextureID);
        glBindTexture(GL_TEXTURE_2D, terrainTextureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Create Texture Color Data With higher resolution so it looks better ya know
        int LOD = 16;
        int HLOD = LOD;
        int resolution = segments * LOD;
        numberofPoints = (resolution + 1) * (resolution + 1);

        texData = new float[numberofPoints * 3];
        sampleOffset = (float)size / resolution;
        halfSize = size / 2.0f;
		noise.SetFractalOctaves(noise.GetOctaves() + 1);

        //Generate Data
        for (size_t row = 0; row <= resolution; row++)
        {
            float zPos = row * sampleOffset - halfSize;

            for (size_t col = 0; col <= resolution; col++)
            {
                //Texture Data
                float xPos = col * sampleOffset - halfSize;
                int index = (((resolution - row) * (resolution + 1)) + col) * 3;
                float noiseValue = (noise.GetNoise(xPos, zPos) + 1.0f) / 2.0f;

                float distanceFromCenter = sqrt(xPos * xPos + zPos * zPos) - 0.4;
                //noiseValue -= distanceFromCenter;

                glm::vec3 color = glm::vec3(noiseValue);

                if (textures.size() > 0)
                {
                    for (auto tex : textures)
                    {
                        if (noiseValue > tex.second)
                        {
                            color = tex.first->Sample(glm::vec2((float)col * HLOD / resolution, (float)row * HLOD / resolution));
                            break;
						}
                    }
                }


                texData[index] = color.r;
                texData[index + 1] = color.g;
                texData[index + 2] = color.b;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, resolution + 1, resolution + 1, 0, GL_RGB, GL_FLOAT, texData);
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