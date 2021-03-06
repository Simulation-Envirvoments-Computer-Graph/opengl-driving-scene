#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace Assets
{
    class Shader
    {
    public:
        Shader(std::string computePath);
        Shader(std::string vertexPath, std::string fragmentPath);
        Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath);
        Shader(std::string vertexPath, std::string tcsPath, std::string tesPath, std::string geomPath, std::string fragmentPath);
        ~Shader();

        void attachShader(GLuint shaderType, std::string shaderPath, std::string shaderTypeStr);
        void checkCompileErrors(GLuint shader, std::string type);
        void use() const;

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec2(const std::string &name, float x, float y) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w) const;
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
    
    private:
        unsigned int mID;
    };
}