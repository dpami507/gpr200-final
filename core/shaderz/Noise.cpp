#include "Noise.h"

namespace shaderz {
    float Noise::GenNoise(glm::vec2 p, float frequency)
    {
        float cellSize = 1.0f / frequency;
        glm::vec2 gridCoords = glm::floor(p / cellSize);
        glm::vec2 localPos = p / cellSize - gridCoords; // Better than mod

        // Smooth interpolation curve (cosine interpolation)
        localPos = (glm::vec2(1.0f) - glm::cos(ew::PI * localPos)) * 0.5f;

        // Get random values at the four corners of the grid cell
        float topLeft = rand(gridCoords + glm::vec2(0.0f, 0.0f));
        float topRight = rand(gridCoords + glm::vec2(1.0f, 0.0f));
        float bottomLeft = rand(gridCoords + glm::vec2(0.0f, 1.0f));
        float bottomRight = rand(gridCoords + glm::vec2(1.0f, 1.0f));

        // Bilinear interpolation
        float topEdge = glm::mix(topLeft, topRight, localPos.x);
        float bottomEdge = glm::mix(bottomLeft, bottomRight, localPos.x);

        return glm::mix(topEdge, bottomEdge, localPos.y);
    }

    float Noise::PerlinNoise(glm::vec2 position, int octaveCount)
    {
        float persistence = 0.3f;
        float accumulatedNoise = 0.0f;
        float totalAmplitude = 0.0f;
        float currentFrequency = 4.0f;
        float currentAmplitude = 1.0f;

        for (int octave = 0; octave < 50; octave++)
        {
            accumulatedNoise += currentAmplitude * GenNoise(position, currentFrequency);
            currentFrequency *= 2.0f;
            totalAmplitude += currentAmplitude;
            currentAmplitude *= persistence;

            if (octave == octaveCount) break;
        }

        float normalizedNoise = accumulatedNoise / totalAmplitude;
        return normalizedNoise * normalizedNoise * normalizedNoise * normalizedNoise;
    }
}