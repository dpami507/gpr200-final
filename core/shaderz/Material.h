#ifndef MATERIAL_H
#define MATERIAL_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"

namespace shaderz {
    class LitMaterial
    {
    public:
        LitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, glm::vec3 color, float ambient, float diffuse, float specular, float shininess);
        void use();
    private:
        Shader* shader;
        Texture2D* texture;
        glm::vec2 uvTiling;
        glm::vec3 color;
        float ambient;
        float diffuse;
        float specular;
        float shininess;
    };

    class UnlitMaterial
    {
    public:
        UnlitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, glm::vec3 color);
        void use();
    private:
        Shader* shader;
        Texture2D* texture;
        glm::vec2 uvTiling;
        glm::vec3 color;
    };
}
#endif