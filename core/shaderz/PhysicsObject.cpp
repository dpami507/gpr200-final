#include "PhysicsObject.h"

namespace shaderz {
	PhyiscsObject::PhyiscsObject(Mesh& mesh, float mass, bool useGravity) : Object(mesh)
	{
		this->useGravity = useGravity;
		this->mass = mass;
		std::cout << "Created Physics Object\n";
	}

	void PhyiscsObject::AddForce(glm::vec3& force)
	{
		glm::vec3 forceToAdd = force / mass;
		acceleration += forceToAdd;
	}

	void PhyiscsObject::draw(bool drawAsPoints, bool drawWireframe)
	{
		if (useGravity)
		{
			glm::vec3 deltaGravity = gravity * Time::deltaTime;
			glm::vec3 forces = acceleration + deltaGravity;
			velocity += forces;
			acceleration = glm::vec3(0);
		}

		transform.position += velocity * Time::deltaTime;
		mesh->draw(drawAsPoints, drawWireframe);
	}
}