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

    void Terrain::DrawOnNoiseTexture(glm::vec2 pos, float height, int width)
    {
        //Move position so its all positive
        pos += glm::vec2(size / 2.0f);

        //Get index from position
        int xIndex = (int)((pos.x / size) * segments);
        int yIndex = (int)((pos.y / size) * segments);

        //Create hump instead of spire :)
        for (size_t i = 0; i < width; i++)
        {
            for (size_t j = 0; j < width; j++)
            {
				//Get new index by going through a square around the point
				int x = xIndex + i - (width / 2);
				int y = yIndex + j - (width / 2);

				//Clamp to bounds
                if (x < 0 || x > segments || y < 0 || y > segments) continue;

                //New index
				int idx = (y * (segments + 1)) + x;

				//Get distance from center and calculate falloff
				float dist = glm::distance(glm::vec2(xIndex, yIndex), glm::vec2(x, y));
				float falloff = glm::clamp(1.0f - (dist / (width / 2.0f)), 0.0f, 1.0f);

				//Increase Height with falloff
				noiseData[idx] += height * falloff;
            }
        }

        //Remake mesh
        MeshData terrainData = createTerrain(size, segments, heightScale, noiseData);
        this->mesh = new Mesh(terrainData);

        //Reload texture
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenTextures(1, &noiseID);
        glBindTexture(GL_TEXTURE_2D, noiseID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, segments + 1, segments + 1, 0, GL_RED, GL_FLOAT, noiseData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

	//Constructor to set up noise, segments and textures
	Terrain::Terrain(const Noise& noise, float size, float heightScale, int segments) : Object(*mesh)
	{
		this->noise = noise;
        this->segments = segments;
		this->size = size;
		this->heightScale = heightScale;

        GenerateNoiseTexture(size, segments);

		MeshData terrainData = createTerrain(size, segments, heightScale, noiseData);
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
	void Terrain::load(float size, int segments, float heightScale, Noise noise)
	{
        this->noise = noise;
		this->segments = segments;
		this->size = size;
		this->heightScale = heightScale;

        delete[] noiseData;
		noiseData = nullptr;

        GenerateNoiseTexture(size, segments);

		MeshData terrainData = createTerrain(size, segments, heightScale, noiseData);
		mesh->load(terrainData);
	}
}