#include "Material.h"

//David Amidon
namespace shaderz {

	GLuint whiteTextureID;

	void DefaultTexture()
	{
		std::cout << "Creating Default Texture\n";
		unsigned char data[] = { 255,255,255,255 };

		glGenTextures(1, &whiteTextureID);
		glBindTexture(GL_TEXTURE_2D, whiteTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	LitMaterial::LitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, const glm::vec3& color, const float& ambient, const float& diffuse, const float& specular, const float& shininess)
	{
		this->shader = shader;
		this->texture = texture.first;
		this->uvTiling = texture.second;
		this->color = color;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
		DefaultTexture();
	}

	void LitMaterial::updateMaterialSettings(const float& diffuse, const float& specular, const float& shininess)
	{
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

		shader->setVec2("uvTiling", uvTiling);

		if (texture != nullptr)
			texture->Bind(0);
		else
			glBindTexture(GL_TEXTURE_2D, whiteTextureID);
	}

	
	UnlitMaterial::UnlitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, glm::vec3 color)
	{
		this->shader = shader;
		this->texture = texture.first;
		this->uvTiling = texture.second;
		this->color = color;
		DefaultTexture();
	}

	void UnlitMaterial::use()
	{
		shader->use();
		shader->setVec3("color", color);

		shader->setVec2("uvTiling", uvTiling);

		if (texture != nullptr)
			texture->Bind(0);
		else
			glBindTexture(GL_TEXTURE_2D, whiteTextureID);
	}
}