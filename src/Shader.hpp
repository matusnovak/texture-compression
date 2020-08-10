#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <optional>
#include <string>

namespace Example {
class Shader {
public:
    Shader(const std::string& vertSource, const std::string& fragSource, const std::optional<std::string>& geomSource);
    ~Shader();

    void checkShaderStatus(GLuint shader) const;
    void checkProgramStatus() const;
    void destroy();
    void use() const;
    void setInt(const std::string& location, int value) const;
    void setFloat(const std::string& location, float value) const;
    void setVec2(const std::string& location, const glm::vec2& value) const;
    void setVec3(const std::string& location, const glm::vec3& value) const;
    void setVec4(const std::string& location, const glm::vec4& value) const;
    void setMat4(const std::string& location, const glm::mat4x4& value) const;
    void drawArrays(const GLenum mode, const GLsizei count) const;

    GLuint get() const {
        return program;
    }

private:
    GLuint vertex;
    GLuint fragment;
    GLuint geometry;
    GLuint program;
};
} // namespace Example
