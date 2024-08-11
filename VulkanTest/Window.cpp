#include "Window.h"

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GraphicsInstance.h"
#include "HelloTriangleApp.h"

#include <imgui_impl_glfw.h>

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // tells glfw it isn't being created for use with OpenGL

    // nullptr for windowed not fullscreen, and nullptr for standalone window not shared
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    // give glfw window a pointer to this class for use in callbacks
    glfwSetWindowUserPointer(window, this);

    // callback for ensuring swapchain is recreated when the glfw window is resized
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

std::vector<const char*> Window::getRequiredInstanceExtensions() const {
    // get extensions required for a vulkan instance to be used with glfw
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}

VkResult Window::createWindowSurface(const std::unique_ptr<GraphicsInstance>& instance, VkSurfaceKHR* surface) const {
    return glfwCreateWindowSurface(instance->getInstance(), window, nullptr, surface);
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

bool Window::wasResized() const {
    return resized;
}

void Window::resetResized() {
    resized = false;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::waitEvents() {
    glfwWaitEvents();
}

void Window::getFramebufferSize(int* width, int* height) {
    glfwGetFramebufferSize(window, width, height);
}

void Window::imguiInit() {
    constexpr bool installCallbacks = true; // installs the callbacks for mouse movement and such for imgui before forwarding to already implemented user callbacks
    ImGui_ImplGlfw_InitForVulkan(window, installCallbacks);
}

void Window::imguiNewFrame() {
    ImGui_ImplGlfw_NewFrame();
}

void Window::imguiShutdown() {
    ImGui_ImplGlfw_Shutdown();
}

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    Window* customWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    customWindow->resized = true;
}
