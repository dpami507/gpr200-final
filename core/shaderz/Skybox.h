#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"

#include "Shader.h"

namespace shaderz {
	//David Amidon
	class Skybox
	{
	public:
		Skybox(Shader& skyboxShader);
		unsigned int loadCubemap();
		void createSkybox();
		void bind();
		void draw();
	private:
		unsigned int skyboxVAO, skyboxVBO;
		unsigned int cubemapTexture;
		unsigned int textureID;
		int width, height, nrChannels;
		Shader* skyboxShader;
		std::vector<std::string> faces
		{
			"assets/skybox/right.jpg",
			"assets/skybox/left.jpg",
			"assets/skybox/top.jpg",
			"assets/skybox/bottom.jpg",
			"assets/skybox/front.jpg",
			"assets/skybox/back.jpg"
		};
	};
}

#endif