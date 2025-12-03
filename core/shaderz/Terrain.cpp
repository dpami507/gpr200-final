#include "Terrain.h"

namespace shaderz {
    void Terrain::GenerateNoiseTexture(float size, int segments)
    {
        std::cout << "Creating Noise Texture\n";

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

                float noiseValue = noise.octavePerlin(xPos, zPos);

				float distanceFromCenter = sqrt(xPos * xPos + zPos * zPos) - 0.4;
                //noiseValue -= distanceFromCenter;

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

	//Constructor to set up noise, segments and textures
	Terrain::Terrain(const Noise& noise, float size, int segments) : Object(*mesh)
	{
		this->noise = noise;
        this->segments = segments;

        GenerateNoiseTexture(size, segments);

		MeshData terrainData = createTerrain(size, segments, noiseData);
		this->mesh = new Mesh(terrainData);

		std::cout << "Created Terrain Object\n";
	}
    Terrain::~Terrain()
    {
        delete[] noiseData;
        noiseData = nullptr;

		delete mesh;
		mesh = nullptr;
	}


    //Call mesh to draw
	void Terrain::draw(bool drawAsPoints, bool drawWireframe)
	{
		mesh->draw(drawAsPoints, drawWireframe);
	}

	//Reloads MeshData to update values
	void Terrain::load(float size, int segments, Noise noise)
	{
        this->noise = noise;

        delete[] noiseData;
		noiseData = nullptr;

        GenerateNoiseTexture(size, segments);

		MeshData terrainData = createTerrain(size, segments, noiseData);
		mesh->load(terrainData);
	}
}