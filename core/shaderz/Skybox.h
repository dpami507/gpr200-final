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
		Skybox(Shader& skyboxShader, std::string hdrFile);
		unsigned int loadHDR(std::string hdrFile);
		void createSkybox();
		void bind();
		void draw();
	private:
		unsigned int skyboxVAO, skyboxVBO;
		unsigned int captureFBO, captureRBO;
		unsigned int hdrTexture;
		unsigned int envCubemap;
		unsigned int textureID;
		int width, height, nrChannels;
		Shader* skyboxShader;
		std::string hdrFile;
	};
}

#endif