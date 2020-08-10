#include "Vao.hpp"
#include <algorithm>

using namespace Example;

Vao::Vao() : ref(0) {
    glGenVertexArrays(1, &ref);
}

Vao::~Vao() {
    if (ref) {
        glDeleteVertexArrays(1, &ref);
    }
}

void Vao::bind() const {
    glBindVertexArray(ref);
}

Vao::Vao(Vao&& other) noexcept : ref(0) {
    swap(other);
}

void Vao::swap(Vao& other) noexcept {
    std::swap(ref, other.ref);
}

Vao& Vao::operator=(Vao&& other) noexcept {
    if (this != &other) {
        swap(other);
    }
    return *this;
}
