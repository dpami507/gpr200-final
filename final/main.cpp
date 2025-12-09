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
//#include "./shaderz/Camera.h"
#include "./shaderz/Transform.h"
#include "./shaderz/Time.h"
#include "./shaderz/VertexGen.h"
#include "./shaderz/Object.h"
#include "./shaderz/PhysicsObject.h"
#include "./shaderz/Material.h"
#include "./shaderz/Skybox.h"
#include "./shaderz/Terrain.h"
#include "./shaderz/tetraz/sandbox.h"

using namespace shaderz;

#define STB_IMAGE_IMPLEMENTATION
#include "ew/external/stb_image.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int objCount = 4;

//Light Settings
glm::vec3 lightColor = glm::vec3(1, 1, 1);
float lightStrength = 1;
float lightFalloff = 10;

//Sphere
int sphereSubdivision = 8;
float sphereRadius = 1;

//Terrain
int terrainSubdivision = 256;
float heightScale = 1;
float terrainSize = 3;
float frequency = 1;
int octaveCount = 4;
int brushSize = 16;

//Water settings
float waterHeight = 0.015;
float waterWidth = 6;
float waterSpeed = 0.05;

//Drawing Options
int currShade = 2;
const char* itemNames[3] = {
  "UVs",
  "Normals",
  "Shaded"
};
bool wireframe = false;
bool point = false;

//Material for terrain
struct Material
{
	Texture2D* color;
	Texture2D* ao;
	Texture2D* normal;
	Texture2D* roughness;
	float threshold;
	float uvTile;
};

// terrain buffer
Terrain* gTerrain = nullptr;

int main() {
	//Create Window
	printf("Creating World...\n");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	GLFWwindow* window = glfwCreateWindow(shaderz::SCREEN_WIDTH, shaderz::SCREEN_HEIGHT, "final... what is this a countdown?", NULL, NULL);
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
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
	Texture2D grassRough("assets/materials/grass/GrassRough.jpg", GL_NEAREST, GL_REPEAT);

	//Rock Texture
	Texture2D rockColor("assets/materials/rock/RockColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockAO("assets/materials/rock/RockAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockNorm("assets/materials/rock/RockNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D rockRough("assets/materials/rock/RockRough.jpg", GL_NEAREST, GL_REPEAT);

	//Sand Texture
	Texture2D sandColor("assets/materials/sand/SandColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D sandAO("assets/materials/sand/SandAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D sandNorm("assets/materials/sand/SandNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D sandRough("assets/materials/sand/SandRough.jpg", GL_NEAREST, GL_REPEAT);

	//Dirt Texture
	Texture2D dirtColor("assets/materials/dirt/DirtColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D dirtAO("assets/materials/dirt/DirtAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D dirtNorm("assets/materials/dirt/DirtNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D dirtRough("assets/materials/dirt/DirtRough.jpg", GL_NEAREST, GL_REPEAT);

	//Snow Texture
	Texture2D snowColor("assets/materials/snow/SnowColor.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D snowAO("assets/materials/snow/SnowAO.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D snowNorm("assets/materials/snow/SnowNorm.jpg", GL_NEAREST, GL_REPEAT);
	Texture2D snowRough("assets/materials/snow/SnowRough.jpg", GL_NEAREST, GL_REPEAT);

	//Water Noise
	Texture2D waterNoise("assets/cellularNoise.jpg", GL_LINEAR, GL_REPEAT);

	UnlitMaterial waterMaterial(&waterShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));
	UnlitMaterial blankMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));
	UnlitMaterial lightMaterial(&unlitShader, { nullptr, glm::vec2(1) }, glm::vec3(255, 255, 255));

	//Create Primitive Meshes
	Mesh sphere(createSphere(sphereRadius, sphereSubdivision));
	Mesh plane(createPlane(terrainSize, terrainSize, terrainSubdivision, true));

	//Create noise for terrain
	Noise perlinNoise(octaveCount, frequency);

	//Create array of terrain textures
	std::vector<Material> terrainTextures;
	terrainTextures.push_back({ &dirtColor,	 &dirtAO,	&dirtNorm,	&dirtRough,	0.0, 10 });
	terrainTextures.push_back({ &sandColor,	 &sandAO,	&sandNorm,	&sandRough,	0.45, 5 });
	terrainTextures.push_back({ &grassColor, &grassAO,  &grassNorm, &grassRough, 0.5, 5 });
	terrainTextures.push_back({ &rockColor,	 &rockAO,	&rockNorm,	&rockRough,	0.6, 5 });
	terrainTextures.push_back({ &snowColor,	 &snowAO,	&snowNorm,	&snowRough,	0.75, 5 });

	//Create Terrain Object
	Terrain terrainObj(perlinNoise, terrainSize, heightScale, terrainSubdivision);
	gTerrain = &terrainObj;

	//Objects
	Object heightObj(plane);
	Object waterObj(plane);

	//Create Skybox
	Shader skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	Shader hdrToCubemapShader("assets/shaders/hdrToCubemap.vert", "assets/shaders/hdrToCubemap.frag");
	Skybox skybox(skyboxShader, hdrToCubemapShader, "assets/sky.hdr");

	glViewport(0, 0, shaderz::SCREEN_WIDTH, shaderz::SCREEN_HEIGHT);

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Update Time
		Time::Update();

		//Clear framebuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Setup objects
		{	
			//Set TerrainShader
			terrainShader.use();
			terrainShader.setMat4("projectionView", camera.getProjectionView());

			//Set up texture for Terrain
			terrainShader.use();
			terrainShader.setFloat("frequency", frequency);
			terrainShader.setInt("octaves", octaveCount + 2);
			terrainShader.setVec3("viewPos", camera.getPosition());
			//Set light properties
			terrainShader.setVec3("lightDirection", glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f)));
			terrainShader.setVec3("lightColor", glm::vec3(1.0f));
			terrainShader.setFloat("lightIntensity", 1.0f);
			//Bind skybox
			skybox.bind(0);
			terrainShader.setInt("skybox", 0);

			//Set up noise texture
			terrainObj.BindNoiseTexture(1);
			terrainShader.setInt("noiseTexture", 1);

			terrainShader.setFloat("heightScale", heightScale);
			//Bind terrain textures
			for(int i = 0; i < terrainTextures.size(); i++)
			{
				int baseUnit = 2 + (i * 4);

				terrainTextures[i].color->Bind(baseUnit + 0);
				terrainShader.setInt("materials[" + std::to_string(i) + "].colorTex",	baseUnit + 0);

				terrainTextures[i].ao->Bind(baseUnit + 1);
				terrainShader.setInt("materials[" + std::to_string(i) + "].aoTex",		baseUnit + 1);

				terrainTextures[i].normal->Bind(baseUnit + 2);
				terrainShader.setInt("materials[" + std::to_string(i) + "].normalTex",	baseUnit + 2);

				terrainTextures[i].roughness->Bind(baseUnit + 3);
				terrainShader.setInt("materials[" + std::to_string(i) + "].roughTex",	baseUnit + 3);

				terrainShader.setFloat("materials[" + std::to_string(i) + "].threshold", terrainTextures[i].threshold);
				terrainShader.setFloat("materials[" + std::to_string(i) + "].uvTile", terrainTextures[i].uvTile);
			}
			terrainShader.setInt("numMaterials", terrainTextures.size());
			//Draw Terrain
			terrainObj.transform.position = glm::vec3(0, 0, 0);
			terrainShader.setMat4("model", terrainObj.transform.GetModel());
			terrainObj.draw(point, wireframe);

			/////////////////////////////////////////////////

			//Set UnlitShader
			unlitShader.use();
			unlitShader.setMat4("projectionView", camera.getProjectionView());

			//Set up texture to show heightmap
			blankMaterial.use();
			terrainObj.BindNoiseTexture(0);
			unlitShader.setInt("texture1", 0);

			heightObj.transform.position = glm::vec3(0, -heightScale / 2, 0);
			unlitShader.setMat4("model", heightObj.transform.GetModel());
			heightObj.draw(point, wireframe);

			/////////////////////////////////////////////////

			//Set WaterShader
			waterShader.use();
			waterShader.setMat4("projectionView", camera.getProjectionView());
			waterShader.setVec3("viewPos", camera.getPosition());
			//Wave settings
			waterShader.setFloat("uTime", Time::time);
			waterShader.setFloat("width", waterWidth);
			waterShader.setFloat("height", waterHeight);
			waterShader.setFloat("speed", waterSpeed);
			//Skybox
			skybox.bind(0);
			waterShader.setInt("skybox", 0);
			//Water Noise
			waterNoise.Bind(1);
			waterShader.setInt("cellularNoise", 1);

			//Create Water Plane
			waterMaterial.use();
			waterObj.transform.position = glm::vec3(0, 0.45f, 0);

			waterShader.setMat4("model", waterObj.transform.GetModel());
			waterObj.draw(point, wireframe);
		}

		//Draw Skybox
		skybox.draw(camera.getView(), camera.getProjection());

		//ImGui
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

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
				ImGui::SliderInt("Terrain Segments", &terrainSubdivision, 1, 2048); 
				ImGui::SliderFloat("Terrain Size", &terrainSize, 1, 16);
				ImGui::SliderFloat("Hegiht Scale", &heightScale, 1, 3);
				ImGui::SliderInt("Octave Count", &octaveCount, 1, 8);
				ImGui::SliderFloat("Frequency", &frequency, 0.0, 8.0);

				ImGui::Separator();
				ImGui::SliderInt("Brush Size", &brushSize, 1, 128);
				if (ImGui::Button("Regenerate Terrain"))
				{
					Noise perlinNoise(octaveCount, frequency);

					terrainObj.load(terrainSize, terrainSubdivision, heightScale, perlinNoise);
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

// Tetra made this file
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Terrain object is global in main()
		extern Terrain terrainObj;    // Forward declare
		glm::vec3 hit = shaderz::rayCollision(gTerrain, camera, (int)mouseX, (int)mouseY);
		if (hit == glm::vec3(1234567, 1234567, 1234567))
			return; // it didn't hit anything and shouldn't check anything

		float brushStrength = 0.0f;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			brushStrength = -10.0f * Time::deltaTime;
		else
			brushStrength = 10.0f * Time::deltaTime;

		gTerrain->DrawOnNoiseTexture(glm::vec2(hit.x, -hit.z), brushStrength, brushSize);

		std::cout << "Clicked terrain vertex: "
			<< hit.x << ", " << hit.y << ", " << -hit.z << std::endl;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	camera->cameraZoom(yoffset);
}