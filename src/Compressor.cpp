#include "Compressor.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>
#include <stdexcept>
#include <vector>

using namespace Example;

static const std::string SHADER_FRAG = R"(#version 330 core
in vec2 v_texCoords;

out vec4 fragmentColor;

uniform sampler2D tex;

void main() {
    fragmentColor = texture(tex, v_texCoords);
}
)";

static const std::string SHADER_VERT = R"(#version 330 core
layout(location = 0) in vec2 position;

out vec2 v_texCoords;

void main() {
    vec2 coords = (position + 1.0) * 0.5;
    v_texCoords = vec2(coords.x, 1.0 - coords.y);
    gl_Position = vec4(position, 1.0, 1.0);
}
)";

static const float FULL_SCREEN_QUAD[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

Compressor::Result::Result(const GLuint target, const GLuint ref) : target(target), ref(ref) {
}

Compressor::Result::~Result() {
    if (ref) {
        glDeleteTextures(1, &ref);
    }
}

void Compressor::Result::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ref);
}

Compressor::Result::Result(Result&& other) noexcept : target(0), ref(0) {
    swap(other);
}

void Compressor::Result::swap(Result& other) noexcept {
    std::swap(target, other.target);
    std::swap(ref, other.ref);
}

Compressor::Result& Compressor::Result::operator=(Result&& other) noexcept {
    if (this != &other) {
        swap(other);
    }
    return *this;
}

Compressor::Compressor() : shader(SHADER_VERT, SHADER_FRAG, std::nullopt) {
    shader.setInt("tex", 0);

    vao.bind();
    vbo.bind();
    vbo.bufferData(reinterpret_cast<const uint8_t*>(FULL_SCREEN_QUAD), sizeof(FULL_SCREEN_QUAD));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

Compressor::~Compressor() = default;

Compressor::Result Compressor::compress(const std::string& filename, const GLuint target, const GLsizei width) {
    auto levels = static_cast<int>(std::log2(width)) - 2;

    // Load the source image
    int imgWidth, imgHeigth, imgChannels;
    auto* image = stbi_load(filename.c_str(), &imgWidth, &imgHeigth, &imgChannels, STBI_rgb_alpha);

    if (!image) {
        throw std::runtime_error("Failed to open image file");
    }

    if (imgChannels != 3 && imgChannels != 4) {
        throw std::runtime_error("Image must be RGB or RGBA");
    }

    // Create the destination texture
    GLuint destination;
    glGenTextures(1, &destination);
    glBindTexture(GL_TEXTURE_2D, destination);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1);

    // Create texture from the source image
    GLuint source;
    glGenTextures(1, &source);
    glBindTexture(GL_TEXTURE_2D, source);
    if (imgChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgWidth, imgHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, imgWidth, imgHeigth, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    stbi_image_free(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Create framebuffer with renderbuffer
    GLuint fbo, fboColor, fboDepth;

    glGenRenderbuffers(1, &fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, width);

    glGenTextures(1, &fboColor);
    glBindTexture(GL_TEXTURE_2D, fboColor);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDER, fboDepth);

    // Render the texture to the framebuffer, this will create mipmaps
    for (auto level = 0; level < levels; level++) {
        const auto w = width >> level;
        glBindTexture(GL_TEXTURE_2D, fboColor);
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA8, w, w, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, level);

        glViewport(0, 0, w, w);

        vao.bind();
        glBindTexture(GL_TEXTURE_2D, source);
        shader.use();
        shader.drawArrays(GL_TRIANGLES, 2 * 3);
    }

    // Set the mipmap levels for the fbo color texture
    glBindTexture(GL_TEXTURE_2D, fboColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1);

    GLint totalBytes = 0;

    // Copy pixels as compressed texture
    for (auto level = 0; level < levels; level++) {
        const auto w = width >> level;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColor, level);

        glBindTexture(GL_TEXTURE_2D, destination);
        glCopyTexImage2D(GL_TEXTURE_2D, level, target, 0, 0, w, w, 0);

        // Optionally download the texture into a raw array.
        // You can use this to save the generated texture into a file.
        // This has to be done per mipmap level!

        GLint compressedSize;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);
        totalBytes += compressedSize;

        // std::vector<uint8_t> pixels;
        // pixels.resize(compressedSize);
        // glGetCompressedTexImage(GL_TEXTURE_2D, level, pixels.data());
    }

    std::cout << "Total bytes: " << totalBytes << std::endl;

    // Cleanup
    glDeleteTextures(1, &source);
    glDeleteTextures(1, &fboColor);
    glDeleteRenderbuffers(1, &fboDepth);
    glDeleteFramebuffers(1, &fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Result result(GL_TEXTURE_2D, destination);
    return result;
}
