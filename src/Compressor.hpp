#pragma once

#include "Shader.hpp"
#include "Vao.hpp"
#include "Vbo.hpp"

namespace Example {
class Compressor {
public:
    class Result {
    public:
        Result(GLuint target, GLuint ref);
        Result(const Result& other) = delete;
        Result(Result&& other) noexcept;
        ~Result();

        void swap(Result& other) noexcept;
        Result& operator=(const Result& other) = delete;
        Result& operator=(Result&& other) noexcept;

        void bind() const;

        GLuint getRef() const {
            return ref;
        }

        GLuint getTarget() const {
            return target;
        }

    private:
        GLuint target;
        GLuint ref;
    };

    Compressor();
    ~Compressor();

    Result compress(const std::string& filename, GLuint target, GLsizei width);

private:
    Shader shader;
    Vao vao;
    Vbo vbo;
};
} // namespace Example
