#include "Skybox.h"

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
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	Skybox::Skybox(Shader& skyboxShader, Shader& conversionShader, const std::string& hdrFile)
	{
		this->skyboxShader = &skyboxShader;
		this->conversionShader = &conversionShader;

		skyboxShader.use();
		skyboxShader.setInt("environmentMap", 0);

		hdrTexture = loadHDR(hdrFile);
		cubemapTexture = convertToCubemap();
		
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
	//Load the HDR
	unsigned int Skybox::loadHDR(const std::string& hdrFile)
	{
		std::cout << "Loading HDR\n";

		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		stbi_set_flip_vertically_on_load(true);
		float* data = stbi_loadf(hdrFile.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			std::cout << "HDR loaded path: " << hdrFile << std::endl;
			std::cout << "HDR resolution: " << width << " x " << height << "\n";
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load HDR at path: " << hdrFile << std::endl;
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		CheckError("HDR_LOAD");

		return hdrTexture;
	}
	//Convert the hdr to a cubemap
	unsigned int Skybox::convertToCubemap()
	{
		std::cout << "Converting HDR to Cubemap\n";

		//Conversion Shader
		conversionShader->use();
		conversionShader->setInt("equirectangularMap", 0);
		conversionShader->setMat4("projection", captureProjection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		glViewport(0, 0, 512, 512);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			std::cout << "Converting face " << i << std::endl;

			conversionShader->setMat4("view", captureViews[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTexture, 0);
			
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << "Framebuffer not complete for face " << i << " - status: 0x" << std::hex << status << std::dec << "\n";
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			CheckError("SIDE");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		CheckError("CUBEMAP");

		return cubemapTexture;
	}

	//Bind the cubemap texture
	void Skybox::bind()
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	}

	void Skybox::draw()
	{
		glDepthFunc(GL_LEQUAL);

		glBindVertexArray(skyboxVAO);
		bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
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