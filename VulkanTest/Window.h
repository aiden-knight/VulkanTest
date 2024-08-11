#pragma once
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

class Surface;
class GraphicsInstance;

struct GLFWwindow;

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class Window {
public:
    Window();
    ~Window();

	std::vector<const char*> getRequiredInstanceExtensions() const;

    VkResult createWindowSurface(const std::unique_ptr<GraphicsInstance>& instance, VkSurfaceKHR* surface) const;

    bool shouldClose();
    bool wasResized() const;
    void resetResized();

    void pollEvents();
    void waitEvents();
    void getFramebufferSize(int* width, int* height);

    void imguiInit();
    void imguiNewFrame();
    void imguiShutdown();
private:
    /// <summary>
    /// glfw callback for window resized or minimised
    /// </summary>
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
    /// <summary>
    /// Pointer to the glfw window in use
    /// </summary>
    GLFWwindow* window = nullptr;

    bool resized = false;
};