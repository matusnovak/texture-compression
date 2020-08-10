#pragma once

#include <GLFW/glfw3.h>

namespace Example {
class Window {
public:
    Window();
    ~Window();

    void run();

private:
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* window;
    int tupleIndex;
    bool shouldGenerate;
};
} // namespace Example
