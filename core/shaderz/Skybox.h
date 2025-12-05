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

//this is a change

namespace shaderz {
	//David Amidon
	class Skybox
	{
	public:
		Skybox(Shader& skyboxShader, Shader& hdrToCubemapShader, const std::string& hdrFile);
		void createSkybox();

		void loadHDR();
		void createFramebuffer();
		void SetupCubemap();
		void HDRtoCubemap();

		void bind(int slot = 0);
		void draw(glm::mat4 view, glm::mat4 projection);

		void CheckError(std::string location);

	private:
		//Framebuffer
		unsigned int captureFBO;
		unsigned int captureRBO;

		//Set up cubemap
		unsigned int envCubemap;

		//Skybox
		unsigned int skyboxVAO, skyboxVBO;

		//Hdr texture data
		std::string hdrFile;
		unsigned int hdrTexture;
		int width, height, nrChannels;

		//Shaders
		Shader* skyboxShader;
		Shader* hdrToCubemapShader;
	};
}

#endif