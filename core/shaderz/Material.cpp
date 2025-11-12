#include "Material.h"

namespace shaderz {
	LitMaterial::LitMaterial(Shader* shader, Texture2D* texture, glm::vec3 color, float ambient, float diffuse, float specular, float shininess)
	{
		this->shader = shader;
		this->texture = texture;
		this->color = color;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	void LitMaterial::use()
	{
		shader->use();

		shader->setVec3("color", color);

		shader->setFloat("material.ambient", ambient);
		shader->setFloat("material.diffuse", diffuse);
		shader->setFloat("material.specular", specular);
		shader->setFloat("material.shininess", shininess);

		if (texture != nullptr)
			texture->Bind(0);
		else
			texture->DefaultTexture();
	}

	
	UnlitMaterial::UnlitMaterial(Shader* shader, Texture2D* texture, glm::vec3 color)
	{
		this->shader = shader;
		this->texture = texture;
		this->color = color;
	}

	void UnlitMaterial::use()
	{
		shader->use();
		shader->setVec3("color", color);

		if(texture != nullptr)
			texture->Bind(0);
		else
			texture->DefaultTexture();
	}
}