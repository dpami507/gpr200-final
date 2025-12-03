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
int terrainSubdivision = 16;
float terrainSize = 1;
float frequency = 4.0;
int octaveCount = 4;

float waterHeight = 0.01;
float waterWidth = 7.5;
float waterSpeed = 0.3;

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
	Shader waterShader("assets/shaders/water.vert", "assets/shaders/water.frag");
	Shader terrainShader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");

	//Grass Texture
	Texture2D grassColor("assets/materials/grass/GrassColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D grassAO("assets/materials/grass/GrassAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D grassNorm("assets/materials/grass/GrassNorm.jpg", GL_NEAREST, GL_REPEAT);

	//Rock Texture
	Texture2D rockColor("assets/materials/rock/RockColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockAO("assets/materials/rock/RockAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockNorm("assets/materials/rock/RockNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockRough("assets/materials/rock/RockRough.jpg", GL_NEAREST, GL_REPEAT);

	//Sand Texture
	Texture2D sandColor("assets/materials/sand/SandColor.jpg", GL_NEAREST, GL_REPEAT);

	//Dirt Texture
	Texture2D dirtColor("assets/materials/dirt/DirtColor.jpg", GL_NEAREST, GL_REPEAT);

	//Snow Texture
	Texture2D snowColor("assets/materials/snow/SnowColor.jpg", GL_NEAREST, GL_REPEAT);
 
	//Gold Texture
	Texture2D goldColor("assets/GoldColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldMetal("assets/GoldMetal.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldNorm("assets/GoldNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D goldRough("assets/GoldRough.jpg", GL_NEAREST, GL_REPEAT);

	PBRMaterial landMaterial(&pbrShader, glm::vec2(1.0f), &grassColor, nullptr, &grassNorm, nullptr, &grassAO);
	PBRMaterial goldMaterial(&pbrShader, glm::vec2(1.0f), &goldColor, &goldRough, &goldNorm, &goldMetal, nullptr);
	UnlitMaterial waterMaterial(&waterShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));
	UnlitMaterial blankMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));
	UnlitMaterial lightMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));

	//Create Primitive Meshes
	Mesh sphere(createSphere(sphereRadius, sphereSubdivision));
	Mesh plane(createPlane(terrainSize, terrainSize, terrainSubdivision, true));


	Noise perlinNoise(octaveCount, frequency);

	std::vector<std::pair<Texture2D*, std::pair<float, float>>> terrainTextures;

	terrainTextures.push_back({ &dirtColor,		{0.0,  5} });
	terrainTextures.push_back({ &sandColor,		{0.45, 5} });
	terrainTextures.push_back({ &grassColor,	{0.5,  5} });
	terrainTextures.push_back({ &rockColor,		{0.6,  5} });
	terrainTextures.push_back({ &snowColor,		{0.75, 5} });

	Terrain terrainObj(perlinNoise, terrainSize, terrainSubdivision);

	//Light Object
	Object lightObject(sphere);
	lightObject.transform.position = glm::vec3(0.0, 2.0, 0.0);
	lightObject.transform.rotation = glm::vec3(0.0);
	lightObject.transform.scale = glm::vec3(0.5);

	//Objects
	Object heightObj(plane);
	Object waterObj(plane);

	//Create Skybox
	Shader skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	Skybox skybox(skyboxShader, "assets/warm.hdr");

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Update Time
		Time::Update();

		//Clear framebuffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Setup objects
		{	
			skybox.bind();

			//Set TerrainShader
			terrainShader.use();
			terrainShader.setMat4("projectionView", camera.getProjectionView());

			//Set UnlitShader
			unlitShader.use();
			unlitShader.setMat4("projectionView", camera.getProjectionView());

			//Set WaterShader
			waterShader.use();
			waterShader.setMat4("projectionView", camera.getProjectionView());

			//Set up texture for Terrain
			terrainShader.use();
			terrainShader.setFloat("frequency", frequency);
			terrainShader.setInt("octaves", octaveCount + 1);

			for(int i = 0; i < terrainTextures.size(); i++)
			{
				terrainTextures[i].first->Bind(i);
				terrainShader.setInt("textures[" + std::to_string(i) + "]", i);
				terrainShader.setFloat("thresholds[" + std::to_string(i) + "]", terrainTextures[i].second.first);
				terrainShader.setFloat("uvTile[" + std::to_string(i) + "]", terrainTextures[i].second.second);
			}
			terrainShader.setInt("numTextures", terrainTextures.size());

			terrainObj.transform.position = glm::vec3(0, 0, 0);
			terrainShader.setMat4("model", terrainObj.transform.GetModel());
			terrainObj.draw(point, wireframe);

			//Set up texture to show heightmap
			blankMaterial.use();
			terrainObj.BindNoiseTexture(0);
			unlitShader.setInt("texture1", 0);

			heightObj.transform.position = glm::vec3(0, 0, 0);
			unlitShader.setMat4("model", heightObj.transform.GetModel());
			heightObj.draw(point, wireframe);

			//Create Water Plane
			waterMaterial.use();
			waterObj.transform.position = glm::vec3(0, 0.45f, 0);

			waterShader.setFloat("uTime", Time::time);
			waterShader.setFloat("w", waterWidth);
			waterShader.setFloat("h", waterHeight);
			waterShader.setFloat("s", waterSpeed);

			waterShader.setMat4("model", waterObj.transform.GetModel());
			waterObj.draw(point, wireframe);
		}

		//Set up Light Object
		{
			//Use Shader
			lightMaterial.use();
			unlitShader.setMat4("model", lightObject.transform.GetModel());
			lightObject.draw(point, wireframe);
		}

		skybox.draw(camera.getView(), camera.getProjection());

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
				ImGui::SliderInt("Terrain Segments", &terrainSubdivision, 1, 512); 
				ImGui::SliderFloat("Terrain Size", &terrainSize, 1, 16);
				ImGui::SliderInt("Octave Count", &octaveCount, 1, 8);
				ImGui::SliderFloat("Frequency", &frequency, 1, 8);

				if (ImGui::Button("Regenerate Terrain"))
				{
					Noise perlinNoise(octaveCount, frequency);

					terrainObj.load(terrainSize, terrainSubdivision, perlinNoise);
					plane.load(createPlane(terrainSize, terrainSize, terrainSubdivision, true));
				}
			}

			if (ImGui::CollapsingHeader("Water Settings"))
			{
				ImGui::SliderFloat("Water Height", &waterHeight, 0, 1);
				ImGui::SliderFloat("Water Width", &waterWidth, 0, 32);
				ImGui::SliderFloat("Water Speed", &waterSpeed, 0, 4);
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