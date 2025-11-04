#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ew/external/glad.h"
#include "../ew/external/stb_image.h"

namespace shaderz {
    class Transform
    {
    public:
        Transform();
        Transform(glm::vec3 translateXYZ, glm::vec3 rotateXYZ, glm::vec3 scaleXYZ);
        ~Transform();

        void Scale(glm::vec3 scaleXYZ);
        void Scale(float x, float y, float z);
        void Scale(float scale);

        void ScaleX(float x) { scale.x += x; };
        void ScaleY(float y) { scale.y += y; };
        void ScaleZ(float z) { scale.z += z; };

        void Rotate(glm::vec3 rotateXYZ);
        void Rotate(float x, float y, float z);
        void Rotate(float rotate);

        void RotateX(float x) { rotation.x += x; };
        void RotateY(float y) { rotation.y += y; };
        void RotateZ(float z) { rotation.z += z; };

        void Translate(glm::vec3 translateXYZ);
        void Translate(float x, float y, float z);
        void Translate(float translate);

        void TranslateX(float x) { position.x += x; };
        void TranslateY(float y) { position.y += y; };
        void TranslateZ(float z) { position.z += z; };

        void UpdateTransform();

        glm::mat4 GetModel();

        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 rotation = glm::vec3(0, 0, 0);
        glm::vec3 scale = glm::vec3(0, 0, 0);
    private:
        glm::mat4 model = glm::mat4(1.0f);
    };
}

#endif