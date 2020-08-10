// clang-format off
#include <glad/glad.h> // Needs to be first
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Window.hpp"
#include <exception>
#include <iostream>
#include <random>
#include "Shader.hpp"
#include "Vbo.hpp"
#include "Vao.hpp"
#include "Compressor.hpp"
// clang-format on

// Imported from GLAD library
#define GL_COMPRESSED_RED_RGTC1_EXT 0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1_EXT 0x8DBC
#define GL_COMPRESSED_RED_GREEN_RGTC2_EXT 0x8DBD
#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT 0x8DBE
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

using namespace Example;

static const std::vector<std::tuple<std::string, GLuint>> tuples = {
    {"RGB_S3TC_DXT1", GL_COMPRESSED_RGB_S3TC_DXT1_EXT},
    {"RGBA_S3TC_DXT1", GL_COMPRESSED_RGBA_S3TC_DXT1_EXT},
    {"RGBA_S3TC_DXT3", GL_COMPRESSED_RGBA_S3TC_DXT3_EXT},
    {"RGBA_S3TC_DXT5", GL_COMPRESSED_RGBA_S3TC_DXT5_EXT},
    {"RED_RGTC1", GL_COMPRESSED_RED_RGTC1_EXT},
    {"SIGNED_RED_RGTC1", GL_COMPRESSED_SIGNED_RED_RGTC1_EXT},
    {"RED_GREEN_RGTC2", GL_COMPRESSED_RED_GREEN_RGTC2_EXT},
    {"SIGNED_RED_GREEN_RGTC2", GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT}};

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
    v_texCoords = (position + 1.0) * 0.5;
    gl_Position = vec4(position, 1.0, 1.0);
}
)";

static const float FULL_SCREEN_QUAD[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Window::Window() : window(nullptr), tupleIndex(0), shouldGenerate(true) {
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void Window::run() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw");
    }

    // Basic GLFW window stuff
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(512, 512, "Texture Compression", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSwapInterval(1);

    // You need to enable these
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_BLEND);
    glEnable(GL_RENDERBUFFER);
    glEnable(GL_FRAMEBUFFER);

    // Default blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    // Shader to render the texture on the screen
    Shader shader(SHADER_VERT, SHADER_FRAG, std::nullopt);
    Vao vao;
    Vbo vbo;

    vao.bind();
    vbo.bind();
    vbo.bufferData(reinterpret_cast<const uint8_t*>(FULL_SCREEN_QUAD), sizeof(FULL_SCREEN_QUAD));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // The thing that will compress the texture
    Compressor compressor;
    Compressor::Result result(0, 0);

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if (shouldGenerate) {
            const auto& tuple = tuples[tupleIndex];
            const auto& name = std::get<0>(tuple);
            const auto target = std::get<1>(tuple);
            std::cout << "Generating as: " << name << "(" << target << ")" << std::endl;
            result = compressor.compress("lena.png", target, 512);
            shouldGenerate = false;
        }

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vao.bind();
        result.bind();
        shader.use();
        shader.setInt("tex", 0);
        shader.drawArrays(GL_TRIANGLES, 2 * 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Window::errorCallback(const int error, const char* description) {
    std::cerr << "error: " << error << " description: " << description << std::endl;
}

void Window::keyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
    (void)scancode;
    (void)mods;

    auto& self = *reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        self.shouldGenerate = true;
        self.tupleIndex++;
        if (self.tupleIndex == tuples.size()) {
            self.tupleIndex = 0;
        }
    }
}

int main(const int argc, char** argv) {
    try {
        Window window;
        window.run();
        return EXIT_SUCCESS;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
