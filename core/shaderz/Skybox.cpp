#include "Skybox.h"

//this is a change

//David Amidon
namespace shaderz {
	float skyboxVertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f, // bottom-left
		 1.0f,  1.0f, -1.0f, // top-right
		 1.0f, -1.0f, -1.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f, // top-right
		-1.0f, -1.0f, -1.0f, // bottom-left
		-1.0f,  1.0f, -1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f, // bottom-left
		 1.0f, -1.0f,  1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f, // top-right
		 1.0f,  1.0f,  1.0f, // top-right
		-1.0f,  1.0f,  1.0f, // top-left
		-1.0f, -1.0f,  1.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, // top-right
		-1.0f,  1.0f, -1.0f, // top-left
		-1.0f, -1.0f, -1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f, // top-left
		 1.0f, -1.0f, -1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f, // top-right         
		 1.0f, -1.0f, -1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f, // top-left
		 1.0f, -1.0f,  1.0f, // bottom-left     
		 // bottom face
		 -1.0f, -1.0f, -1.0f, // top-right
		  1.0f, -1.0f, -1.0f, // top-left
		  1.0f, -1.0f,  1.0f, // bottom-left
		  1.0f, -1.0f,  1.0f, // bottom-left
		 -1.0f, -1.0f,  1.0f, // bottom-right
		 -1.0f, -1.0f, -1.0f, // top-right
		 // top face
		 -1.0f,  1.0f, -1.0f, // top-left
		  1.0f,  1.0f , 1.0f, // bottom-right
		  1.0f,  1.0f, -1.0f, // top-right     
		  1.0f,  1.0f,  1.0f, // bottom-right
		 -1.0f,  1.0f, -1.0f, // top-left
		 -1.0f,  1.0f,  1.0f, // bottom-left        
	};

	Skybox::Skybox(Shader& skyboxShader, const std::string& hdrFile)
	{
		this->skyboxShader = &skyboxShader;

		this->skyboxShader->use();
		this->skyboxShader->setInt("skybox", 0);

		createSkybox();
		loadCubemap();
	}

	void Skybox::loadCubemap()
	{
		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

		for (unsigned int i = 0; i < faces.size(); i++)
		{
			stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				int format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3) ? GL_RGB : (nrChannels == 2) ? GL_RG : GL_RED;

				std::cout << "Cubemap loaded path: " << faces[i] << " (channels: " << nrChannels << ")" << std::endl;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		{
			std::cout << "Converting face " << i << std::endl;

			conversionShader->setMat4("view", captureViews[i]);


			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Framebuffer not complete for face " << i << " - status: 0x" << std::hex << status << std::dec << "\n";
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			CheckError("SIDE");
			renderCube();
			//glBindVertexArray(skyboxVAO);
			//glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		CheckError("CUBEMAP");
	}
	//Create irradiance map
	void Skybox::createIrradianceMap()
	{
		std::cout << "Creating Irradiance Map\n";

		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		irradianceShader->use();
		irradianceShader->setInt("environmentMap", 0);
		irradianceShader->setMat4("projection", captureProjection);

		//Idk which one to use
		glActiveTexture(GL_TEXTURE0);
		//glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		//glBindVertexArray(skyboxVAO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			std::cout << "Creating irradianceCubemap face " << i << std::endl;

			irradianceShader->setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
			//glBindVertexArray(skyboxVAO);
			//glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		CheckError("IRRADIANCE");
	}
	//Create the prefilter map
	void Skybox::createPrefilterMap()
	{
		std::cout << "Creating Prefilter Cubemap\n";

		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void Skybox::createSkybox()
	{
		std::cout << "Creating Skybox\n";

		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);

		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		CheckError("SKYBOX");
	}

	//Bind the cubemap texture
	void Skybox::bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	}

	void Skybox::draw(const glm::mat4& projection, const glm::mat4& view)
	{
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		skyboxShader->use();
		skyboxShader->setMat4("view", glm::mat4(glm::mat3(view)));
		skyboxShader->setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		bind();

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		CheckError("DRAW");
	}

	void Skybox::CheckError(std::string location)
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string error = "";

			switch (err)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}

			std::cout << "ERROR_" << location << ": " << error << "\n";
			// Process/log the error.
		}
	}
}