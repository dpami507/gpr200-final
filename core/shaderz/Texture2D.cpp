#include "Texture2D.h"

namespace shaderz {
	Texture2D::Texture2D(const char* filePath, int filterMode, int wrapMode) {

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		//Set texture wrapping/filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		stbi_set_flip_vertically_on_load(true);

		//Load Image
		unsigned char* data = stbi_load(filePath, &m_width, &m_height, &m_nrChannels, 0);

		//Check and Generate Mipmap
		if (data)
		{
			int format = (m_nrChannels == 4) ? GL_RGBA : (m_nrChannels == 3) ? GL_RGB : (m_nrChannels == 2) ? GL_RG : GL_RED;

			glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_id);
		std::cout << "Texture " << m_id << " Terminated" << std::endl;
	}
	void Texture2D::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
}