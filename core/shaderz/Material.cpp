#include "Material.h"

//David Amidon
namespace shaderz {

	GLuint defaultBlackID;
	GLuint defaultWhiteID;

	void DefaultWhite()
	{
		std::cout << "Creating Default White\n";
		unsigned char data[] = { 255,255,255,255 };

		glGenTextures(1, &defaultWhiteID);
		glBindTexture(GL_TEXTURE_2D, defaultWhiteID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void DefaultBlack()
	{
		std::cout << "Creating Default Black\n";
		unsigned char data[] = { 0,0,0,255 };

		glGenTextures(1, &defaultBlackID);
		glBindTexture(GL_TEXTURE_2D, defaultBlackID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	PBRMaterial::PBRMaterial(Shader* shader, glm::vec2 uv, 
		Texture2D* albedo, Texture2D* roughness, Texture2D* normal, 
		Texture2D* metallic, Texture2D* ao)
	{
		this->shader = shader;
		this->uvTiling = uv;

		this->albedo = albedo;
		this->roughness = roughness;
		this->normal = normal;
		this->metallic = metallic;
		this->ao = ao;

		DefaultBlack();
	}
	void PBRMaterial::use()
	{
		shader->use();

		shader->setVec2("uvTiling", uvTiling);

		shader->setInt("albedoMap", 0);
		if (albedo != nullptr)
			albedo->Bind(0);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, defaultBlackID);
		}

		shader->setInt("roughnessMap", 1);
		if (roughness != nullptr)
			roughness->Bind(1);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, defaultBlackID);
		}

		shader->setInt("normalMap", 2);
		if (normal != nullptr)
			normal->Bind(2);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, defaultBlackID);
		}

		shader->setInt("metallicMap", 3);
		if (metallic != nullptr)
			metallic->Bind(3);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, defaultBlackID);
		}

		shader->setInt("aoMap", 4);
		if (ao != nullptr)
			ao->Bind(4);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, defaultBlackID);
		}
	}

	LitMaterial::LitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, 
		const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, 
		const glm::vec3& specular, const float& shininess)
	{
		this->shader = shader;
		this->texture = texture.first;
		this->uvTiling = texture.second;
		this->color = color;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;

		DefaultWhite();
	}

	void LitMaterial::updateMaterialSettings(const glm::vec3& diffuse, const glm::vec3& specular, const float& shininess)
	{
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	void LitMaterial::use()
	{
		shader->use();

		shader->setVec3("color", color);

		shader->setVec3("material.ambient", ambient);
		shader->setVec3("material.diffuse", diffuse);
		shader->setVec3("material.specular", specular);
		shader->setFloat("material.shininess", shininess);

		shader->setVec2("uvTiling", uvTiling);

		shader->setInt("texture1", 0);
		if (texture != nullptr)
			texture->Bind(0);
		else
		{
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, defaultWhiteID);
		}
	}

	
	UnlitMaterial::UnlitMaterial(Shader* shader, std::pair<Texture2D*, glm::vec2> texture, glm::vec3 color)
	{
		this->shader = shader;
		this->texture = texture.first;
		this->uvTiling = texture.second;
		this->color = color;

		DefaultWhite();
	}

	void UnlitMaterial::use()
	{
		shader->use();
		shader->setVec3("color", color);
		shader->setVec2("uvTiling", uvTiling);

		shader->setInt("texture1", 0);
		if (texture != nullptr)
			texture->Bind(0);
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, defaultWhiteID);
		}
	}
}