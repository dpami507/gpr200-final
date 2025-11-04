#include "Camera.h"

namespace shaderz {
	Camera::Camera(GLFWwindow* window, glm::uvec3 pos)
	{
		cameraWindow = window;
		cameraPos = pos;
		glfwGetWindowSize(cameraWindow, &width, &height);
		updateCameraVectors();
	}

	void Camera::controlCamera(float deltaTime)
	{
		//Camera Speed
		const float cameraMultiplier = (glfwGetKey(cameraWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.5 : 1.0;
		const float cameraSpeed = 5.0f * cameraMultiplier * deltaTime;

		//Change camera view
		if (glfwGetKey(cameraWindow, GLFW_KEY_P) == GLFW_PRESS)
		{
			isOrthographic = false;
		}
		if (glfwGetKey(cameraWindow, GLFW_KEY_O) == GLFW_PRESS)
		{
			isOrthographic = true;
		}

		//Cursor Lock State
		rightMouseDown = (glfwGetMouseButton(cameraWindow, GLFW_MOUSE_BUTTON_2)) ? true : false;
		auto lockState = (rightMouseDown) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(cameraWindow, GLFW_CURSOR, lockState);

		//Movement
		if (glfwGetKey(cameraWindow, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(cameraWindow, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(cameraWindow, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraRight;
		if (glfwGetKey(cameraWindow, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraRight;

		//Up/Down
		if (glfwGetKey(cameraWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos += cameraSpeed * worldUp;
		if (glfwGetKey(cameraWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			cameraPos -= cameraSpeed * worldUp;
	}

	void Camera::processMouseMovement(float xoffset, float yoffset)
	{
		if (!rightMouseDown) return;

		//Change movement based on sensitivity
		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		//Add movement to offset
		yaw += xoffset;
		pitch += yoffset;

		//Clamp pitch
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		updateCameraVectors();
	}

	void Camera::cameraZoom(float yoffset)
	{
		//Change fov and clamp it
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 120.0f)
			fov = 120.0f;
	}

	glm::mat4 Camera::getView() const
	{
		glm::mat4 view;
		glm::vec3 target = cameraPos + cameraFront;
		view = glm::lookAt(cameraPos, target, cameraUp);

		return view;
	}

	glm::mat4 Camera::getProjection()
	{
		glm::mat4 projection;

		float newFov = 0;
			
		if (isOrthographic)
		{
			float scale = 20.0f;
			newFov = scale * (121.0f - fov);

			projection = glm::ortho(-width  / newFov,		//Left
									 width  / newFov,		//Right
									-height / newFov,		//Bottom
									 height / newFov,		//Top
									-1000.0f, 1000.0f);		//Clipping
		}
		else
		{
			newFov = fov;
			projection = glm::perspective(glm::radians(newFov), (float)width / (float)height, 0.1f, 100.0f);
		}

		return projection;
	}

	glm::mat4 Camera::getProjectionView()
	{
		return getProjection() * getView();
	}

	void Camera::updateCameraVectors() 
	{
		//Create rotation values in unit sphere to create driection vector
		glm::vec3 direction(0, 0, 0);
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		//Set camera locals
		cameraFront = glm::normalize(direction);
		cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
		cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));
	}
}

