#ifndef MATERIAL_H
#define MATERIAL_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

#include "Texture2D.h"
#include "Shader.h"

//David Amidon
namespace shaderz {
    class PBRMaterial
    {
    public:
        PBRMaterial(Shader* shader, glm::vec2 uv, Texture2D* albedo, Texture2D* roughness, Texture2D* normal, Texture2D* metallic, Texture2D* ao);
        void use();
    private:
        Shader* shader;
        glm::vec2 uvTiling;

        Texture2D* albedo;
        Texture2D* roughness;
        Texture2D* normal;
        Texture2D* metallic;
        Texture2D* ao;
    };

    class LitMaterial
    {
    public:
        LitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float& shininess);
        void updateMaterialSettings(const glm::vec3& diffuse, const glm::vec3& specular, const float& shininess);
        void use();
    private:
        Shader* shader;
        Texture2D* texture;
        glm::vec2 uvTiling;
        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
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