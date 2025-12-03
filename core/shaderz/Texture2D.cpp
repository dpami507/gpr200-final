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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		stbi_set_flip_vertically_on_load(true);

		//Load Image
		m_data = stbi_load(filePath, &m_width, &m_height, &m_nrChannels, 0);

		//Check and Generate Mipmap
		if (m_data)
		{
			int format = (m_nrChannels == 4) ? GL_RGBA : (m_nrChannels == 3) ? GL_RGB : (m_nrChannels == 2) ? GL_RG : GL_RED;

			glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, m_data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	}
	Texture2D::~Texture2D()
	{
		if(m_data)
		{
			stbi_image_free(m_data);
			m_data = nullptr;
		}
		if (m_id)
		{
			glDeleteTextures(1, &m_id);
			std::cout << "Texture " << m_id << " Terminated" << std::endl;
		}
	}
	glm::vec4 Texture2D::Sample(const glm::vec2& uv)
	{
		if (!m_data || m_width <= 0 || m_height <= 0) return glm::vec4(0.0f);

		float u = uv.x - floor(uv.x);
		float v = uv.y - floor(uv.y);

		int x = (int)(u * m_width);
		int y = (int)(v * m_height);

		if (x < 0) x = 0; if (x >= m_width) x = m_width - 1;
		if (y < 0) y = 0; if (y >= m_height) y = m_height - 1;

		int index = (y * m_width + x) * m_nrChannels;
		unsigned char* p = &m_data[index];

		float r = (m_nrChannels >= 1) ? (p[0] / 255.0f) : 0.0f;
		float g = (m_nrChannels >= 2) ? (p[1] / 255.0f) : r;
		float b = (m_nrChannels >= 3) ? (p[2] / 255.0f) : r;
		float a = (m_nrChannels == 4) ? (p[3] / 255.0f) : 1.0f;

		return glm::vec4(r, g, b, a);
	}
	void Texture2D::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
}