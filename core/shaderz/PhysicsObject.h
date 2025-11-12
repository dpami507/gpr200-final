#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include "Transform.h"
#include "VertexGen.h"
#include "Object.h"
#include "Time.h"

namespace shaderz {
	class PhysicsObject : public Object
	{
	public:
		PhysicsObject(Mesh& mesh, float mass = 1, bool useGravity = true);
		void AddForce(glm::vec3& force);

		void draw(bool drawAsPoints = false, bool drawWireframe = false);

		float mass;

		bool useGravity = true;
		const glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

		glm::vec3 velocity = glm::vec3(0.0);
		glm::vec3 acceleration = glm::vec3(0.0);
	private:
		
	};
}

#endif