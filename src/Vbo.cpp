#include "Vbo.hpp"
#include <algorithm>

using namespace Example;

Vbo::Vbo() : ref(0) {
    glGenBuffers(1, &ref);
}

Vbo::~Vbo() {
    if (ref) {
        glDeleteBuffers(1, &ref);
    }
}

void Vbo::bufferData(const uint8_t* data, const size_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, ref);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Vbo::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ref);
}

Vbo::Vbo(Vbo&& other) noexcept : ref(0) {
    swap(other);
}

void Vbo::swap(Vbo& other) noexcept {
    std::swap(ref, other.ref);
}

Vbo& Vbo::operator=(Vbo&& other) noexcept {
    if (this != &other) {
        swap(other);
    }
    return *this;
}
