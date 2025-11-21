#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

namespace shaderz {
	//David Amidon
	class Skybox
	{
	public:
		Skybox(Shader& skyboxShader, Shader& conversionShader, const std::string& hdrFile);
		unsigned int loadHDR(const std::string& hdrFile);
		unsigned int convertToCubemap();

		void createSkybox();

		void bindIBL(Shader& pbrShader);

		void bind();
		void draw();

		void CheckError(std::string location);


	private:
		unsigned int skyboxVAO, skyboxVBO;
		unsigned int captureFBO, captureRBO;
		unsigned int hdrTexture;
		unsigned int cubemapTexture;
		unsigned int textureID;

		int width, height, nrChannels;

		Shader* skyboxShader;
		Shader* conversionShader;

		std::string hdrFile;
	};
}

#endif