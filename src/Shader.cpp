#include "Shader.hpp"
#include <stdexcept>

using namespace Example;

Shader::Shader(const std::string& vertSource, const std::string& fragSource,
               const std::optional<std::string>& geomSource)
    : vertex(0), fragment(0), geometry(0), program(0) {

    try {
        auto vertexSrc = vertSource.c_str();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSrc, nullptr);
        glCompileShader(vertex);
        checkShaderStatus(vertex);

        auto fragmentSrc = fragSource.c_str();
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSrc, nullptr);
        glCompileShader(fragment);
        checkShaderStatus(fragment);

        if (geomSource.has_value()) {
            auto geometrySrc = geomSource.value().c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &geometrySrc, nullptr);
            glCompileShader(geometry);
            checkShaderStatus(geometry);
        }

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        if (geomSource.has_value()) {
            glAttachShader(program, geometry);
        }
        glLinkProgram(program);
        checkProgramStatus();

    } catch (...) {
        destroy();
        std::rethrow_exception(std::current_exception());
    }
}

Shader::~Shader() {
    destroy();
}

void Shader::checkShaderStatus(const GLuint shader) const {
    char infoLog[512];
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile shader error: " + std::string(infoLog));
    };
}

void Shader::checkProgramStatus() const {
    char infoLog[512];
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        throw std::runtime_error("Failed to link shader error: " + std::string(infoLog));
    };
}

void Shader::destroy() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
    if (vertex) {
        glDeleteShader(vertex);
        vertex = 0;
    }
    if (geometry) {
        glDeleteShader(geometry);
        vertex = 0;
    }
    if (fragment) {
        glDeleteShader(fragment);
        fragment = 0;
    }
}

void Shader::use() const {
    glUseProgram(program);
}

void Shader::setInt(const std::string& location, const int value) const {
    glUniform1i(glGetUniformLocation(program, location.c_str()), value);
}

void Shader::setFloat(const std::string& location, const float value) const {
    glUniform1f(glGetUniformLocation(program, location.c_str()), value);
}

void Shader::setVec2(const std::string& location, const glm::vec2& value) const {
    glUniform2f(glGetUniformLocation(program, location.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string& location, const glm::vec3& value) const {
    glUniform3f(glGetUniformLocation(program, location.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& location, const glm::vec4& value) const {
    glUniform4f(glGetUniformLocation(program, location.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& location, const glm::mat4x4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(program, location.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::drawArrays(const GLenum mode, const GLsizei count) const {
    glDrawArrays(mode, 0, count);
}
