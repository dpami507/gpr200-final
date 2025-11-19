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
		Skybox(Shader& skyboxShader, Shader& irradianceShader, Shader& prefilterShader,
			Shader& brdfShader, Shader& conversionShader, const std::string& hdrFile);
		void loadHDR(const std::string& hdrFile);
		void createSkybox();
		void createEnvCubemap();
		void createFramebuffer();
		void createIrradianceMap();
		void createPrefilterMap();

		void createBRDF();
		void renderQuad();

		void bindIBL(Shader& pbrShader);
		void convertToCubemap();
		void draw(const glm::mat4& projection, const glm::mat4& view);

		unsigned int irradianceMap;
		unsigned int prefilterMap;
		unsigned int brdfLUTTexture;
	private:
		unsigned int skyboxVAO, skyboxVBO;
		unsigned int captureFBO, captureRBO;

		unsigned int envCubemap;
		unsigned int hdrTexture;

		unsigned int textureID;
		int width, height, nrChannels;

		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		Shader* brdfShader;
		Shader* skyboxShader;
		Shader* irradianceShader;
		Shader* conversionShader;
		Shader* prefilterShader;

		std::string hdrFile;
	};
}

#endif