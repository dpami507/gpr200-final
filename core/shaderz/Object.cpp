#include "Object.h"

namespace shaderz {
	Object::Object(Mesh& mesh)
	{
		//Setup mesh
		this->mesh = &mesh;

		//Set origin position
		transform.position = glm::vec3(0.0, 0.0, 0.0);
		transform.rotation = glm::vec3(0.0, 0.0, 0.0);
		transform.scale = glm::vec3(1.0, 1.0, 1.0);
	}

	void Object::draw(bool drawAsPoints, bool drawWireframe)
	{
		mesh->draw(drawAsPoints, drawWireframe);
	}
}