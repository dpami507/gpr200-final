#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <iostream>

#include "../ew/external/glad.h"
#include <glm/glm.hpp>

namespace shaderz {
    class Shader
    {
    public:
        // the program ID
        GLuint ID;

        // constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath);
        // use/activate the shader
        void use();
        // utility uniform functions
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;

        void setVec2(const std::string& name, glm::vec2 vec) const;
        void setVec3(const std::string& name, glm::vec3 vec) const;

        void setMat2(const std::string& name, const glm::mat2& mat) const;
        void setMat3(const std::string& name, const glm::mat3& mat) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}

#endif