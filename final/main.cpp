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
#include "./shaderz/Noise.h"

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

//Material Settings
float ambientK = 0.1;
float diffuseK = 1.0;
float specularK = 1.0;
int shininess = 16;

//Light Settings
glm::vec3 lightColor = glm::vec3(1, 1, 1);
float lightStrength = 1;
float lightFalloff = 10;

//Sphere
int sphereSubdivision = 8;
float sphereRadius = 1;

//Terrain
int terrainSubdivision = 1;
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

	//Create Shaders
	Shader objectShader("assets/lightingShader.vert", "assets/lightingShader.frag");
	Shader lightShader("assets/lightObj.vert", "assets/lightObj.frag");

	//Generate Texture
	Texture2D worldTexture("assets/Brick.jpg", GL_NEAREST, GL_REPEAT);

	//Create Primitive Meshes
	Mesh sphere(createSphere(sphereRadius, sphereSubdivision));

	Noise n;
	std::vector<std::vector<float>> map = n.GeneratePerlinMap(terrainSize, terrainSubdivision, noiseScale, octaveCount, persistence, frequency, amplitude);
	Mesh terrain(createTerrain(terrainSize, heightScale, terrainSubdivision, noiseScale, map));

	//Light Object
	Object lightObject(sphere);
	lightObject.transform.position = glm::vec3(0.0, 2.0, 0.0);
	lightObject.transform.rotation = glm::vec3(0.0);
	lightObject.transform.scale = glm::vec3(0.5);

	//Objects
	Object terrainObj(terrain);

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//TO DO
		// Create Global Settings
		// -Lights (position, color, strength, falloff) <-- probably jsut make a light class
		// 
		// Material Classes (Lit/Unlit)
		// -Take in a texture
		// -Take in a color
		// -Take in Diff, Spec, Shini (Leave out ambient that will be in a global setting)
		// 
		// Landscape
		// -Use some decided noise to make hills and mountains
		// -Using the heights we can set different textures and colors for it
		//

		//Culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Update Time
		Time::Update();

		//Clear framebuffer
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Setup objects
		{
			objectShader.use();

			objectShader.setFloat("material.ambient", ambientK);
			objectShader.setFloat("material.diffuse", diffuseK);
			objectShader.setFloat("material.specular", specularK);
			objectShader.setFloat("material.shininess", shininess);

			//Set LightShader Uniforms
			objectShader.setInt("shadingMode", currShade);

			//Light Settings
			objectShader.setVec3("lightPos", lightObject.transform.position);
			objectShader.setVec3("lightColor", lightColor);

			objectShader.setFloat("lightStrength", lightStrength);
			objectShader.setFloat("lightFalloff", lightFalloff);

			//Camera Projection
			objectShader.setMat4("projectionView", camera.getProjectionView());
			objectShader.setVec3("viewPos", camera.getPosition());
			objectShader.setFloat("uTime", Time::time);

			//Plane
			terrainObj.transform.position = glm::vec3(0, -1, 0);
			terrainObj.transform.scale = glm::vec3(5, 1, 5);
			objectShader.setMat4("model", terrainObj.transform.GetModel());
			terrainObj.draw(point, wireframe);
		}

		//Set up Light Object
		{
			//Use Shader
			lightShader.use();

			//Camera Projection
			lightShader.setMat4("projectionView", camera.getProjectionView());

			//Set Color
			lightShader.setFloat("lightStrength", lightStrength);
			lightShader.setVec3("lightColor", lightColor);

			//Set Model
			lightShader.setMat4("model", lightObject.transform.GetModel());
			lightObject.draw(point, wireframe);
		}

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
				ImGui::SliderFloat("Light Strength", &lightStrength, 0.0f, 2.0f);
				ImGui::SliderFloat("Light Falloff", &lightFalloff, 0.0f, 15.0f);
				ImGui::SliderFloat("Ambient K", &ambientK, 0.0f, 1.0f);
			}

			if (ImGui::CollapsingHeader("Material Settings"))
			{
				ImGui::SliderFloat("Diffuse K", &diffuseK, 0.0f, 1.0f);
				ImGui::SliderFloat("Specular K", &specularK, 0.0f, 1.0f);
				ImGui::SliderInt("Shininess K", &shininess, 2.0f, 1024.0f);
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
					sphere = createSphere(sphereRadius, sphereSubdivision);
				}
			}

			if (ImGui::CollapsingHeader("Terrain Settings"))
			{
				if (ImGui::SliderInt("Terrain Segments", &terrainSubdivision, 1, 512) ||
					ImGui::SliderFloat("Terrain Size", &terrainSize, 1, 16) ||
					ImGui::SliderFloat("Terrain Height", &heightScale, 1, 32) ||
					ImGui::SliderFloat("Noise Scale", &noiseScale, 1, 32) ||
					ImGui::SliderInt("Octave Count", &octaveCount, 1, 8) ||
					ImGui::SliderFloat("Persistence", &persistence, 0, 8) ||
					ImGui::SliderFloat("Frequency", &frequency, 0, 8) ||
					ImGui::SliderFloat("Amplitude", &amplitude, 0, 8))
				{
					Noise n;
					std::vector<std::vector<float>> map = n.GeneratePerlinMap(terrainSize, terrainSubdivision, noiseScale, octaveCount, persistence, frequency, amplitude);
					terrain = createTerrain(terrainSize, heightScale, terrainSubdivision, noiseScale, map);
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