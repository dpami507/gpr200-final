#pragma once

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

		// getter function
		Mesh* getMesh() { return mesh; } // identifier mesh is undefined
	protected: // expected an expression
		Mesh* mesh;
	};
}

#endif // expected a ;