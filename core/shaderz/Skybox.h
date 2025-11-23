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
<<<<<<< Updated upstream
		Skybox(Shader& skyboxShader, const std::string& hdrFile);
		void loadCubemap();
		void createSkybox();

		void bind();
		void draw(const glm::mat4& projection, const glm::mat4& view);
=======
		Skybox(Shader& skyboxShader, Shader& conversionShader, const std::string& hdrFile);
		//unsigned int loadHDR(const std::string& hdrFile);
		//unsigned int convertToCubemap();
		void loadCubemap();

		void createSkybox();

		void bind();
		void draw(glm::mat4 view, glm::mat4 projection);
>>>>>>> Stashed changes

		void CheckError(std::string location);

	private:
		unsigned int skyboxVAO, skyboxVBO;

		unsigned int cubemapTexture;
		int width, height, nrChannels;

		Shader* skyboxShader;

<<<<<<< Updated upstream
=======
		std::string hdrFile;
>>>>>>> Stashed changes
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