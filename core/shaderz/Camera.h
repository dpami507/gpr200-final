#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace shaderz {

	const int SCREEN_WIDTH = 1000;
	const int SCREEN_HEIGHT = 1000;

	class Camera
	{
	public:
		Camera(GLFWwindow* window, glm::uvec3 pos);

		//Camera movement / modification
		void controlCamera(float deltaTime);
		void processMouseMovement(float xoffset, float yoffset);
		void cameraZoom(float yoffset);

		//Getters
		bool getFirstMouse() const { return firstMouse; }
		float getLastX() const { return lastX; }
		float getLastY() const { return lastY; }

		//Setters
		void setFirstMouse(bool firstMouse) { this->firstMouse = firstMouse; }
		void setLastX(float lastX) { this->lastX = lastX; }
		void setLastY(float lastY) { this->lastY = lastY; }

		//Getters
		glm::mat4 getView() const;
		glm::mat4 getProjection();
		glm::mat4 Camera::getProjectionView();
		glm::vec3 getPosition() { return cameraPos; }

	private:
		void updateCameraVectors();
		GLFWwindow* cameraWindow;
		int height = 800, width = 800;

		//Control Variables
		bool firstMouse = true;
		float yaw = -90.0f;
		float pitch = 0.0f;
		float lastX = width / 2.0;
		float lastY = height / 2.0;
		float fov = 60.0f;

		bool rightMouseDown = false;
		bool isOrthographic = false;

		//Camera Variables
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);

		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);

		glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 worldRight = glm::vec3(1.0f, 0.0f, 0.0f);
	};
}