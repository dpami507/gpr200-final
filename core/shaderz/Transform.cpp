#include "Transform.h"

namespace shaderz {
	Transform::Transform(glm::vec3 translateXYZ, glm::vec3 rotateXYZ, glm::vec3 scaleXYZ)
	{
		position = translateXYZ;
		rotation = rotateXYZ;
		scale = scaleXYZ;
	}

	Transform::Transform()
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		rotation = glm::vec3(0.0, 0.0, 0.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
	}

	Transform::~Transform()
	{
		//std::cout << "Killed Transform\n";
	}

	//------------------------------------------------
	//                     Scale                     |
	//------------------------------------------------
	void Transform::Scale(glm::vec3 scaleXYZ)
	{
		scale += scaleXYZ;
	}
	void Transform::Scale(float x, float y, float z)
	{
		scale.x += x;
		scale.y += y;
		scale.z += z;
	}
	void Transform::Scale(float scaleFactor)
	{
		scale += glm::vec3(scaleFactor);
	}

	//------------------------------------------------
	//                   Rotation                    |
	//------------------------------------------------
	void Transform::Rotate(glm::vec3 rotateXYZ)
	{
		rotation += rotateXYZ;
	}
	void Transform::Rotate(float x, float y, float z)
	{
		rotation.x += x;
		rotation.y += y;
		rotation.z += z;
	}
	void Transform::Rotate(float rotate)
	{
		rotation += glm::vec3(rotate);
	}

	//------------------------------------------------
	//                  Translation                  |
	//------------------------------------------------
	void Transform::Translate(glm::vec3 translateXYZ)
	{
		position += translateXYZ;
	}
	void Transform::Translate(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
	}
	void Transform::Translate(float translate)
	{
		position += glm::vec3(translate);
	}

	void Transform::UpdateTransform() 
	{
		//New Model
		glm::mat4 newModel = glm::mat4(1.0);

		//Translate
		newModel = glm::translate(newModel, glm::vec3(position.x, position.y, position.z));

		//Rotate
		newModel = glm::rotate(newModel, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // X-axis
		newModel = glm::rotate(newModel, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Y-axis
		newModel = glm::rotate(newModel, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Z-axis

		//Scale
		newModel = glm::scale(newModel, glm::vec3(scale.x, scale.y, scale.z));

		model = newModel;
	}

	glm::mat4 Transform::GetModel()
	{
		UpdateTransform();
		return model;
	}
}