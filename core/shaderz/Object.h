#ifndef OBJECT_H
#define OBJECT_H

#include "VertexGen.h"
#include "Transform.h"

namespace shaderz {
	class Object
	{
	public:
		Object(Mesh& mesh);
		void draw(bool drawAsPoints = false, bool drawWireframe = false);

		Transform transform;
	protected:
		Mesh* mesh;
	};
}

#endif