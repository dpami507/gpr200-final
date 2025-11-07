#ifndef VERTEXGEN_H
#define VERTEXGEN_H

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "../ew/external/glad.h"
#include "../ew/ewMath/ewMath.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Noise.h"

namespace shaderz {
	//Stores data for a single vertex
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
	};

	//Data structure for storing vertices and indices. Will be passed to Mesh class.
	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	MeshData createPlane(float width, float height, int segments, bool doubleSided = false);
	MeshData createSphere(float radius, int segments);
	MeshData createCylinder(float radius, float height, int segments);
	MeshData createTorus(float radius, float thickness, int segments);
	MeshData createTorus(float radius, float thickness, int segments);
	MeshData createQuad(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topRight, glm::vec3 topLeft);
	MeshData createTerrain(float size, float heightScale, int segments);

	class Mesh {
	public:
		Mesh(const MeshData& meshData);
		void draw(bool drawAsPoints = false, bool drawWireframe = false); //If drawAsPoints is true, use GL_POINTS instead of GL_TRIANGLES for the draw call
	private:
		unsigned int m_vao, m_vbo, m_ebo;
		unsigned int m_numVertices, m_numIndices;
	};
}

#endif 