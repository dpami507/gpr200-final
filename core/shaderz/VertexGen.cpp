#include "VertexGen.h"

namespace shaderz {
	MeshData createPlane(float width, float height, int segments, bool doubleSided)
	{
		MeshData m;
		int numOfVerticies = (segments + 1) * (segments + 1);
		m.vertices.reserve(numOfVerticies);

		//Create Verticies
		for (size_t row = 0; row <= segments; row++)
		{
			for (size_t col = 0; col <= segments; col++)
			{
				Vertex v;

				//Position
				v.pos.x = width * ((float)col / segments) - (width / 2);
				v.pos.y = 0;
				v.pos.z = height * ((float)row / segments) - (height / 2);

				//Normal
				v.normal = glm::vec3(0, 1, 0);

				///UV
				v.uv.x = (float)col / segments;
				v.uv.y = 1 - ((float)row / segments);

				m.vertices.push_back(v);
			}
		}

		//Create indicies
		for (size_t row = 0; row < segments; row++)
		{
			for (size_t col = 0; col < segments; col++)
			{
				//Bottom
				GLuint bl = row * (segments + 1) + col;
				GLuint br = bl + 1;

				//Top
				GLuint tl = bl + (segments + 1);
				GLuint tr = tl + 1;

				//Pushback
				m.indices.push_back(bl);  //  tl-tr
				m.indices.push_back(tl);  //  | /
				m.indices.push_back(tr);  //  bl

				//Pushback
				m.indices.push_back(bl);  //     tr
				m.indices.push_back(tr);  //    / |	
				m.indices.push_back(br);  //  bl-br

				if (doubleSided)
				{
					//Pushback
					m.indices.push_back(bl);  //  tl-tr
					m.indices.push_back(tr);  //  | /
					m.indices.push_back(tl);  //  bl

					//Pushback
					m.indices.push_back(bl);  //     tr
					m.indices.push_back(br);  //    / |
					m.indices.push_back(tr);  //  bl-br
				}
			}
		}

		return m;
	}
	MeshData createSphere(float radius, int segments)
	{
		MeshData m;
		int numOfVerticies = (segments + 1) * (segments + 1);
		m.vertices.reserve(numOfVerticies);

		float stepTheta = 2 * ew::PI / segments;
		float stepPhi = ew::PI / segments;

		//Create Verticies
		for (size_t row = 0; row <= segments; row++)
		{
			//Calc Phi
			float phi = row * stepPhi;

			for (size_t col = 0; col <= segments; col++)
			{
				//Calc Theta
				float theta = col * stepTheta;

				Vertex v;

				//Normal
				v.normal.x = cos(theta) * sin(phi);
				v.normal.y = cos(phi);
				v.normal.z = sin(theta) * sin(phi);

				//Position
				v.pos = v.normal * radius;

				///UV
				v.uv.x = 1 - ((float)col / segments);
				v.uv.y = 1 - ((float)row / segments); //Flip so image isnt upside-down

				m.vertices.push_back(v);
			}
		}

		//Create indicies
		for (size_t row = 0; row < segments; row++)
		{
			for (size_t col = 0; col < segments; col++)
			{
				//Bottom
				int bl = row * (segments + 1) + col;
				int br = bl + 1;

				//Top
				int tl = bl + (segments + 1);
				int tr = tl + 1;

				//Pushback
				m.indices.push_back(bl);  //     tr
				m.indices.push_back(br);  //    / |
				m.indices.push_back(tr);  //  bl-br

				//Pushback
				m.indices.push_back(bl);  //  tl-tr
				m.indices.push_back(tr);  //  | /
				m.indices.push_back(tl);  //  bl
			}
		}

		return m;
	}
	MeshData createCylinder(float radius, float height, int segments)
	{
		MeshData m;

		float stepTheta = 2 * ew::PI / segments;

		float h = height / 2.0;
		Vertex v;

		//Top Center Pivot
		v.pos = glm::vec3(0, h, 0);
		v.normal = glm::vec3(0, 1, 0);
		v.uv = glm::vec2(0.5, 0.5);

		m.vertices.push_back(v);

		//Top
		for (size_t i = 0; i <= segments; i++)
		{
			Vertex v;
			float theta = stepTheta * i;

			v.pos.x = cos(theta) * radius;
			v.pos.y = h;
			v.pos.z = sin(theta) * radius;

			v.normal.x = 0;
			v.normal.y = 1;
			v.normal.z = 0;

			v.uv.x = 0.5f + 0.5f * cos(theta);
			v.uv.y = 0.5f + 0.5f * sin(theta);

			m.vertices.push_back(v);
		}

		int topCenterIdx = 0;
		int bottomCenterIdx = m.vertices.size();

		//Bottom Center Pivot
		v.pos = glm::vec3(0, -h, 0);
		v.normal = glm::vec3(0, -1, 0);
		v.uv = glm::vec2(0.5, 0.5);

		m.vertices.push_back(v);

		//Bottom
		for (size_t i = 0; i <= segments; i++)
		{
			Vertex v;
			float theta = stepTheta * i;

			v.pos.x = cos(theta) * radius;
			v.pos.y = -h;
			v.pos.z = sin(theta) * radius;

			v.normal.x = 0;
			v.normal.y = -1;
			v.normal.z = 0;

			v.uv.x = 0.5f + 0.5f * cos(theta);
			v.uv.y = 0.5f + 0.5f * sin(theta);

			m.vertices.push_back(v);
		}

		int sideStartIdx = m.vertices.size();

		//Side
		for (size_t i = 0; i <= segments; i++)
		{
			Vertex v;
			float theta = stepTheta * i;

			v.pos.x = cos(theta) * radius;
			v.pos.y = -h;
			v.pos.z = sin(theta) * radius;

			v.normal.x = cos(theta);
			v.normal.y = 0;
			v.normal.z = sin(theta);

			v.uv.x = 1 - ((float)i / segments);
			v.uv.y = 0;
			m.vertices.push_back(v);

			v.pos.y = h;
			v.uv.y = 1;

			m.vertices.push_back(v);
		}

		// Top cap indices
		for (int i = 0; i < segments; i++)
		{
			m.indices.push_back(topCenterIdx);
			m.indices.push_back(topCenterIdx + i + 2);
			m.indices.push_back(topCenterIdx + i + 1);
		}
		// Bottom cap indices
		for (int i = 0; i < segments; i++)
		{
			m.indices.push_back(bottomCenterIdx);
			m.indices.push_back(bottomCenterIdx + i + 1);
			m.indices.push_back(bottomCenterIdx + i + 2);
		}
		//Side indicies
		for (int i = 0; i < segments; i++)
		{
			int bl = sideStartIdx + i * 2;
			int tl = bl + 1;

			int br = bl + 2;
			int tr = br + 1;

			m.indices.push_back(bl);	//Bottom Left
			m.indices.push_back(tr);	//Top Right
			m.indices.push_back(br);	//Bottom Right

			m.indices.push_back(bl);	//Bottom Left
			m.indices.push_back(tl);	//Top Left
			m.indices.push_back(tr);	//Top Right
		}
		return m;
	}
	MeshData createTorus(float radius, float thickness, int segments)
	{
		MeshData m;
		int numOfVerticies = (segments + 1) * (segments + 1);
		m.vertices.reserve(numOfVerticies);

		float stepTheta = 2 * ew::PI / segments;
		float stepPhi = 2 * ew::PI / segments;

		//Create Verticies
		for (size_t row = 0; row <= segments; row++)
		{
			//Calc Phi
			float phi = row * stepPhi;

			//Get the point around a circle
			glm::vec3 pointCircle = glm::vec3(cos(phi), 0, sin(phi));

			for (size_t col = 0; col <= segments; col++)
			{
				//Calc Theta
				float theta = col * stepTheta;

				Vertex v;

				//Calculate the point at the radius circle around the thickness circle
				v.pos.x = (pointCircle.x * radius) + (thickness * cos(theta) * pointCircle.x);
				v.pos.y = sin(theta) * thickness;
				v.pos.z = (pointCircle.z * radius) + (thickness * cos(theta) * pointCircle.z);

				//Normals
				v.normal.x = cos(theta) * pointCircle.x;
				v.normal.y = sin(theta);
				v.normal.z = cos(theta) * pointCircle.z;

				///UV
				v.uv.x = (float)col / segments;
				v.uv.y = 1 - ((float)row / segments); //Flip so image isnt upside-down

				m.vertices.push_back(v);
			}
		}

		//Create indicies
		for (size_t row = 0; row < segments; row++)
		{
			for (size_t col = 0; col < segments; col++)
			{
				//Bottom
				int bl = row * (segments + 1) + col;
				int br = bl + 1;

				//Top
				int tl = bl + (segments + 1);
				int tr = tl + 1;

				//Pushback
				m.indices.push_back(bl);  //     tr
				m.indices.push_back(br);  //    / |
				m.indices.push_back(tr);  //  bl-br

				//Pushback
				m.indices.push_back(bl);  //  tl-tr
				m.indices.push_back(tr);  //  | /
				m.indices.push_back(tl);  //  bl
			}
		}

		return m;
	}
	MeshData createQuad(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topRight, glm::vec3 topLeft)
	{
		MeshData m;
		Vertex v;
		glm::vec3 normal = glm::normalize(glm::cross(bottomRight - bottomLeft, topRight - bottomLeft));
		v.normal = normal;

		//bl
		v.pos = bottomLeft;
		v.uv = glm::uvec2(0, 0);
		m.vertices.push_back(v);
		m.indices.push_back(0);

		//br
		v.pos = bottomRight;
		v.uv = glm::uvec2(1, 0);
		m.vertices.push_back(v);
		m.indices.push_back(1);

		//tr
		v.pos = topRight;
		v.uv = glm::uvec2(1, 1);
		m.vertices.push_back(v);
		m.indices.push_back(2);

		normal = glm::normalize(glm::cross(topRight - bottomLeft, topLeft - bottomLeft));
		v.normal = normal;

		//bl
		v.pos = bottomLeft;
		v.uv = glm::uvec2(0, 0);
		m.vertices.push_back(v);
		m.indices.push_back(3);

		//tr
		v.pos = topRight;
		v.uv = glm::uvec2(1, 1);
		m.vertices.push_back(v);
		m.indices.push_back(4);

		//tl
		v.pos = topLeft;
		v.uv = glm::uvec2(0, 1);
		m.vertices.push_back(v);
		m.indices.push_back(5);

		return m;	
	}
	MeshData createTerrain(float size, int segments, float* terrainTexture)
	{
		MeshData m;

		int resolution = segments + 1;
		int numOfVerticies = (segments + 1) * (segments + 1);
		m.vertices.reserve(numOfVerticies);

		float sampleOffset = (float)1 / segments;
		float halfSize = 1 / 2.0f;

		auto getSample = [&](int r, int c) -> float {
			r = std::max(0, std::min(r, segments));
			c = std::max(0, std::min(c, segments));

			int index = ((segments - r) * (segments + 1)) + c;

			float texValue = terrainTexture[index];

			return texValue;
		};

		//Create Verticies
		for (size_t row = 0; row <= segments; row++)
		{
			float zPos = row * sampleOffset - halfSize;

			for (size_t col = 0; col <= segments; col++)
			{
				Vertex v;

				float xPos = col * sampleOffset - halfSize;

				//Position
				v.pos.x = xPos;
				v.pos.y = getSample(row, col);
				v.pos.z = zPos;

				float topHeight = getSample(row + 1, col);
				float bottomLeftHeight = getSample(row - 1, col - 1);
				float bottomRightHeight = getSample(row - 1, col + 1);

				glm::vec3 A = glm::vec3(xPos + sampleOffset, topHeight, zPos);							// 	  A
				glm::vec3 B = glm::vec3(xPos - sampleOffset, bottomLeftHeight, zPos - sampleOffset);	//	 /.\ 
				glm::vec3 C = glm::vec3(xPos - sampleOffset, bottomRightHeight, zPos + sampleOffset);	//	B---C

				v.normal = glm::normalize(glm::cross((B - A), (C - A)));

				///UV
				v.uv.x = (float)col / segments;
				v.uv.y = 1 - ((float)row / segments);

				m.vertices.push_back(v);
			}
		}

		//Create indicies
		for (size_t row = 0; row < segments; row++)
		{
			for (size_t col = 0; col < segments; col++)
			{
				//Bottom
				GLuint bl = row * (segments + 1) + col;
				GLuint br = bl + 1;

				//Top
				GLuint tl = bl + (segments + 1);
				GLuint tr = tl + 1;

				//Pushback
				m.indices.push_back(bl);  //  tl-tr
				m.indices.push_back(tl);  //  | /
				m.indices.push_back(tr);  //  bl

				//Pushback
				m.indices.push_back(bl);  //     tr
				m.indices.push_back(tr);  //    / |	
				m.indices.push_back(br);  //  bl-br
			}
		}

		return m;
	}

	//Creates a new OpenGL VAO, VBO, and EBO, filling the VBO with vertices, EBO with indices
	Mesh::Mesh(const MeshData& meshData)
	{
		m_numVertices = meshData.vertices.size();
		m_numIndices = meshData.indices.size();

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);
		glBindVertexArray(m_vao);

		//VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(Vertex), &meshData.vertices[0], GL_DYNAMIC_DRAW);
		//EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), &meshData.indices[0], GL_DYNAMIC_DRAW);
			
		//Vertex Attributes
		//Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
			
		//UV
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(1);
			
		//Normal
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}

	//Draws a mesh. Just binds the VAO and does a draw call.
	void Mesh::draw(bool drawAsPoints, bool drawWireframe)
	{
		//Wireframe
		glPolygonMode(GL_FRONT, (drawWireframe) ? GL_LINE : GL_FILL);

		//Point
		glPointSize(4);

		glBindVertexArray(m_vao);
		auto type = (drawAsPoints) ? GL_POINTS : GL_TRIANGLES;
		glDrawElements(type, m_numIndices, GL_UNSIGNED_INT, 0);
			
		glBindVertexArray(0);
	}

	//Reloads MeshData to update values
	void Mesh::load(const MeshData& meshData)
	{
		//Set new meshData sizes
		m_numVertices = meshData.vertices.size();
		m_numIndices = meshData.indices.size();
		//VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(Vertex), &meshData.vertices[0], GL_DYNAMIC_DRAW);
		//EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), &meshData.indices[0], GL_DYNAMIC_DRAW);
	}
}