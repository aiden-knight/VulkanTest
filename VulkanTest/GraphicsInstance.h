#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <string>

class Window;

class GraphicsInstance {
public:
    GraphicsInstance(const std::unique_ptr<Window>& window);
    ~GraphicsInstance();

    const VkInstance getInstance() const { return instance; }

private:
    /// <summary>
    /// Checks if required instance extensions are supported
    /// </summary>
    /// <param name="extensions">Required extensions</param>
    bool checkSupported(const std::vector<const char*> extensions) const;

    /// <summary>
    /// Creates and returns a vector of extensions required for the vulkan instance
    /// </summary>
    std::vector<const char*> getRequiredExtensions(const std::unique_ptr<Window>& window) const;

    /// <summary>
    /// Checks if the validation layers are supported on the instance
    /// </summary>
    bool checkValidationLayerSupport() const;

private:
    /// <summary>
    /// Connection between application and the vulkan library
    /// </summary>
    VkInstance instance;
};