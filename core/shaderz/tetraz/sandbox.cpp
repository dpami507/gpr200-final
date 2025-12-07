#include "sandbox.h"

// make a ray based on the mouse
glm::vec3 shaderz::raycastMouse(Camera* cam, int mouse_x, int mouse_y) {
	// normalize each to a range of [-1, 1]
	float xCoord = (2 * mouse_x) / shaderz::SCREEN_WIDTH - 1.0f;
	float yCoord = 1.0f - (2.0f * mouse_y) / shaderz::SCREEN_HEIGHT;
	float zCoord = 1.0f;
	// make the normal ray
	glm::vec3 normalRay = glm::vec3(xCoord, yCoord, zCoord);

	// we want the ray's z to point forewards
	glm::vec4 rayClip = glm::vec4(normalRay.x, normalRay.y, -1.0, 1.0);
	// get camera coordinates and apply them
	glm::vec4 rayWithCam = glm::inverse(cam->getProjection()) * rayClip;
	// manually set z and w values to be forward for the ray
	rayWithCam = glm::vec4(rayWithCam.x, rayWithCam.y, -1.0, 0.0);

	// set the ray to world coordinates
	glm::vec3 rayToWorld = glm::vec3((glm::inverse(cam->getView()) * rayWithCam).x, (glm::inverse(cam->getView()) * rayWithCam).y, (glm::inverse(cam->getView()) * rayWithCam).z);
	// normalize the final ray and send it back
	return glm::normalize(rayToWorld);
}

// check if the ray intersects a triangle using moller trumbore algorithm
bool doesRayIntersectTriangle(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& vertice0, const glm::vec3& vertice1, const glm::vec3& vertice2, float& tOut) {
	//get edges of the trianlge
	glm::vec3 edge1 = vertice1 - vertice0;
	glm::vec3 edge2 = vertice2 - vertice0;

	// get the determinant
	glm::vec3 h = glm::cross(dir, edge2);
	float a = glm::dot(edge1, h);

	// if a is 0 or almost zero, then its parallel to the triangle and is not hitting.
	if (fabs(a) < FLT_EPSILON){ // epilson is to negate it not being 0 by like some weird floating point math 
		return false;
	}

	// this stuff is the main math of the algorithm that goes over my head LMAO
	float f = 1.0f / a;
	glm::vec3 s = orig - vertice0;

	float u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return false;

	glm::vec3 q = glm::cross(s, edge1);
	float v = f * glm::dot(dir, q);
	if (v < 0.0f || u + v > 1.0f)
		return false;

	// compute the ray's distance 
	float t = f * glm::dot(edge2, q);
	if (t > FLT_EPSILON)
	{
		tOut = t; // t out is the distance from the camera to the point it hit
		return true; // IT INTERSECTS!?!?!?!?!? YIPOPEEEEEEEEEEEEEEE
	}
	// did not intersect. sadge
	return false;
}

glm::vec3 shaderz::rayCollision(Object* obj, Camera* cam, int mouse_x, int mouse_y) {

	// get the ray's direction with the power of the other function lol
	glm::vec3 rayDir = shaderz::raycastMouse(cam, mouse_x, mouse_y);
	glm::vec3 rayOrigin = cam->getPosition();

	// get the data of the triangles and it's verticies from the mesh function to save time
	Mesh* mesh = obj->getMesh(); 
	const auto& verts = mesh->getVertices();
	const auto& indices = mesh->getIndices();

	// apply the model's transform
	glm::mat4 model = obj->transform.GetModel();

	float closestT = std::numeric_limits<float>::max();
	glm::vec3 closestVertex(0);

	// go through every triangle (optimization here is likely in the future if I have time, but is likely out of scope considering how long just getting the collision done took)
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		// connect the vertice positions to the world space
		glm::vec3 v0 = glm::vec3(model * glm::vec4(verts[indices[i]].pos, 1));
		glm::vec3 v1 = glm::vec3(model * glm::vec4(verts[indices[i + 1]].pos, 1));
		glm::vec3 v2 = glm::vec3(model * glm::vec4(verts[indices[i + 2]].pos, 1));

		float t;
		// if it hits, keep the distance from the camera to the triangle (t)
		if (doesRayIntersectTriangle(rayOrigin, rayDir, v0, v1, v2, t))
		{
			if (t < closestT)
			{
				closestT = t;

				// get the closest vertex of this trianlge
				glm::vec3 hitPoint = rayOrigin + t * rayDir;

				float d0 = glm::distance(hitPoint, v0);
				float d1 = glm::distance(hitPoint, v1);
				float d2 = glm::distance(hitPoint, v2);

				closestVertex =
					(d0 < d1 && d0 < d2) ? v0 :
					(d1 < d2) ? v1 :
					v2;
			}
		}
	}

	// debug while it doesn't visually do anything in the output window to show it actually collides with stuff
	std::cout << "closest vertex is (" << closestVertex.x << ", " << closestVertex.y << ", " << closestVertex.z << ") " << std::endl;
	return closestVertex; // works even if untouched (no hit -> 0,0,0)
}