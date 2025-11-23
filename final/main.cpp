#include <stdio.h>
#include <math.h>
#include <iostream>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "./shaderz/Shader.h"
#include "./shaderz/Texture2D.h"
#include "./shaderz/Camera.h"
#include "./shaderz/Transform.h"
#include "./shaderz/Time.h"
#include "./shaderz/VertexGen.h"
#include "./shaderz/Object.h"
#include "./shaderz/PhysicsObject.h"
#include "./shaderz/Material.h"
#include "./shaderz/Skybox.h"
#include "./shaderz/Terrain.h"

using namespace shaderz;

#define STB_IMAGE_IMPLEMENTATION
#include "ew/external/stb_image.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

int objCount = 4;

//Light Settings
glm::vec3 lightColor = glm::vec3(1, 1, 1);
float lightStrength = 1;
float lightFalloff = 10;

//Sphere
int sphereSubdivision = 8;
float sphereRadius = 1;

//Terrain
int terrainSubdivision = 5;
float terrainSize = 1;
float heightScale = 1;

float noiseScale = 0.1;
float persistence = 0.3;
float frequency = 4.0;
float amplitude = 1.0;
int octaveCount = 4;

//Drawing Options
int currShade = 2;
const char* itemNames[3] = {
  "UVs",
  "Normals",
  "Shaded"
};
bool wireframe = false;
bool point = false;

int main() {
	//Create Window
	printf("Creating World...\n");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "final... what is this a countdown?", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Create Camera
	Camera camera(window, glm::vec3(0.0f, 0.0f, 3.0f));

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &camera);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//IGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//Create Shaders
	Shader pbrShader("assets/shaders/PBR.vert", "assets/shaders/PBR.frag");
	Shader litShader("assets/shaders/litShader.vert", "assets/shaders/litShader.frag");
	Shader unlitShader("assets/shaders/unlitShader.vert", "assets/shaders/unlitShader.frag");
<<<<<<< Updated upstream
	//Create Skybox
	Shader conversionShader("assets/shaders/cubemap.vert", "assets/shaders/equirectangularToCube.frag");
	Shader skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

	Skybox skybox(skyboxShader, conversionShader, "assets/warm.hdr");

	pbrShader.use();
	pbrShader.setMat4("projection", camera.getProjection());
	skyboxShader.use();
	skyboxShader.setMat4("projection", camera.getProjection());

	skybox.createSkybox();

	//skyboxShader.use();
	//skyboxShader.setInt("environmentMap", 0);
=======
>>>>>>> Stashed changes

	//Grass Texture
	Texture2D grassColor("assets/GrassColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D grassAO("assets/GrassAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D grassNorm("assets/GrassNorm.jpg", GL_NEAREST, GL_REPEAT);
 
	//Gold Texture
	Texture2D goldColor("assets/GoldColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldMetal("assets/GoldMetal.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldNorm("assets/GoldNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldRough("assets/GoldRough.jpg", GL_NEAREST, GL_REPEAT);

	PBRMaterial landMaterial(&pbrShader, glm::vec2(1.0f), &grassColor, nullptr, &grassNorm, nullptr, &grassAO);
	PBRMaterial goldMaterial(&pbrShader, glm::vec2(1.0f), &goldColor, &goldRough, &goldNorm, &goldMetal, nullptr);
	UnlitMaterial waterMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));
	UnlitMaterial lightMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));

	//Create Primitive Meshes
	Mesh sphere(createSphere(sphereRadius, sphereSubdivision));
	Mesh orb(createSphere(1, 32));
	Mesh plane(createPlane(terrainSize, terrainSize, terrainSubdivision, true));

	FastNoiseLite noise;
	noise.SetNoiseType(noise.NoiseType_Perlin);
	noise.SetFrequency(frequency);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(octaveCount);
	Terrain terrainObj(noise, terrainSize, terrainSubdivision);

	//Test Object
	Object orbObj(orb);
	orbObj.transform.position = glm::vec3(5.0, 2.0, 0.0);
	orbObj.transform.rotation = glm::vec3(0.0);
	orbObj.transform.scale = glm::vec3(0.5);

	//Light Object
	Object lightObject(sphere);
	lightObject.transform.position = glm::vec3(0.0, 2.0, 0.0);
	lightObject.transform.rotation = glm::vec3(0.0);
	lightObject.transform.scale = glm::vec3(0.5);

	//Objects
	Object waterObj(plane);

	//Create Skybox
	Shader skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	Skybox skybox(skyboxShader, "assets/warm.hdr");

	pbrShader.use();
	pbrShader.setInt("skybox", 0);
	pbrShader.setMat4("projection", camera.getProjection());

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//TO DO
		// Create Global Settings
		// -Lights (position, color, strength, falloff) <-- probably jsut make a light class
		// -Directional Light
		// -Or IMAGE BASED LIGHTING !!!!!!!
		// -Point Light
		// 
		// Landscape
		// -Using the heights we can set different textures and colors for it
		//
		//

		//Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Update Time
		Time::Update();

		//Clear framebuffer
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Setup objects
		{	
			//Set LitShader
			pbrShader.use();
			pbrShader.setMat4("projectionView", camera.getProjectionView());

			pbrShader.setVec3("lightPos", lightObject.transform.position);
			pbrShader.setVec3("lightColor", lightColor);
			pbrShader.setVec3("viewPos", camera.getPosition());
			pbrShader.setMat4("projectionView", camera.getProjectionView());

<<<<<<< Updated upstream
			//Test
			landMaterial.use();
			orbObj.transform.position = glm::vec3(0, 0, 0);
			pbrShader.setMat4("model", orbObj.transform.GetModel());
			orbObj.draw(point, wireframe);

			//Terrain
			landMaterial.use();
			terrainObj.transform.position = glm::vec3(0, 0, 0);
			pbrShader.setMat4("model", terrainObj.transform.GetModel());
			terrainObj.draw(point, wireframe);

=======
>>>>>>> Stashed changes
			//Set UnlitShader
			unlitShader.use();
			unlitShader.setMat4("projectionView", camera.getProjectionView());
			unlitShader.setFloat("lightStrength", lightStrength);
			unlitShader.setVec3("lightColor", lightColor);

			//Test
			goldMaterial.use();
			orbObj.transform.position = glm::vec3(0, 0, 0);
			pbrShader.setMat4("model", orbObj.transform.GetModel());
			orbObj.draw(point, wireframe);

			//Terrain
			landMaterial.use();
			terrainObj.transform.position = glm::vec3(0, 0, 0);
			pbrShader.setMat4("model", terrainObj.transform.GetModel());
			terrainObj.draw(point, wireframe);

			waterMaterial.use();
			terrainObj.BindTerrainTexture(0);
			unlitShader.setInt("texture1", 0);

			waterObj.transform.position = glm::vec3(0, 0, 0);
			unlitShader.setMat4("model", waterObj.transform.GetModel());
			waterObj.draw(point, wireframe);
		}

		//Set up Light Object
		{
			//Use Shader
			lightMaterial.use();
			unlitShader.setMat4("model", lightObject.transform.GetModel());
			lightObject.draw(point, wireframe);
		}

		//Draw Skybox last
		glDisable(GL_CULL_FACE);
<<<<<<< Updated upstream
		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		skyboxShader.setMat4("projection", camera.getProjection());
		skyboxShader.setMat4("view", camera.getView());
		skybox.draw();
=======
		skybox.draw(camera.getProjection(), camera.getView());
>>>>>>> Stashed changes

		//ImGui
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

			if (ImGui::CollapsingHeader("Light Settings"))
			{
				ImGui::DragFloat3("Light Position", &lightObject.transform.position.x, 0.1f);
				ImGui::ColorEdit3("Light Color", &lightColor.r);
			}

			if (ImGui::CollapsingHeader("Geometry Settings"))
			{
				ImGui::Combo("Shading Modes", &currShade, itemNames, 3);
				ImGui::Checkbox("Wireframe Drawing", &wireframe);
				ImGui::Checkbox("Point Drawing", &point);
			}

			if (ImGui::CollapsingHeader("Mesh Settings"))
			{
				if (ImGui::SliderInt("Sphere Subdivisions", &sphereSubdivision, 3, 64) ||
					ImGui::InputFloat("Sphere Radius", &sphereRadius))
				{
					sphere.load(createSphere(sphereRadius, sphereSubdivision));
				}
			}

			if (ImGui::CollapsingHeader("Terrain Settings"))
			{
				if (ImGui::SliderInt("Terrain Segments", &terrainSubdivision, 1, 512) ||
					ImGui::SliderFloat("Terrain Size", &terrainSize, 1, 16) ||
					ImGui::SliderInt("Octave Count", &octaveCount, 1, 8) ||
					ImGui::SliderFloat("Frequency", &frequency, 0, 8))
				{
					FastNoiseLite noise;

					noise.SetNoiseType(noise.NoiseType_Perlin);
					noise.SetFrequency(frequency);
					noise.SetFractalType(FastNoiseLite::FractalType_FBm);
					noise.SetFractalOctaves(octaveCount);

					terrainObj.load(terrainSize, terrainSubdivision, noise);
					plane.load(createPlane(terrainSize, terrainSize, terrainSubdivision, true));
				}
			}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		//Call camera input update
		camera.controlCamera(Time::deltaTime);
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Destroying the World...\n");
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	//If its the first time get the first and last
	if (camera->getFirstMouse())
	{
		camera->setLastX(xpos);
		camera->setLastY(ypos);
		camera->setFirstMouse(false);
	}

	//Get current and subtract from the last
	float xoffset = xpos - camera->getLastX();
	float yoffset = camera->getLastY() - ypos; //Subtract so controls are not inverted

	//Set new last
	camera->setLastX(xpos);
	camera->setLastY(ypos);

	//Rotate with offset calculated
	camera->processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	camera->cameraZoom(yoffset);
}