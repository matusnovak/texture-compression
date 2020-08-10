#pragma once

#include <cstdint>
#include <glad/glad.h>

namespace Example {
class Vbo {
public:
    Vbo();
    Vbo(const Vbo& other) = delete;
    Vbo(Vbo&& other) noexcept;
    ~Vbo();

    void swap(Vbo& other) noexcept;
    Vbo& operator=(const Vbo& other) = delete;
    Vbo& operator=(Vbo&& other) noexcept;

    void bufferData(const uint8_t* data, const size_t size);
    void bind() const;

    GLuint get() const {
        return ref;
    }

private:
    GLuint ref;
};
} // namespace Example
