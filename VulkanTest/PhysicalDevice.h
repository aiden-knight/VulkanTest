#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <memory>
#include <vector>
#include <set>

class Surface;
class GraphicsInstance;

/// <summary>
/// Struct containing the indices of certain queue families on a physical device
/// </summary>
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamilyOnly; // has transfer capabilities but not graphics

    bool isComplete() const {
        return graphicsFamily.has_value()
            && presentFamily.has_value()
            && transferFamilyOnly.has_value();
    }
};

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class PhysicalDevice {
public:
    PhysicalDevice(const std::unique_ptr<GraphicsInstance>& instance, const std::unique_ptr<Surface>& surface);

    const VkPhysicalDevice getPhysicalDevice() const { return device; }
    const QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices; }
    const SwapchainSupportDetails getSwapchainSupportDetails() const { return swapchainSupportDetails; }
    const VkSampleCountFlagBits getSampleCount() const { return msaaSampleCount; }

    void updateSwapchainSupport(const std::unique_ptr<Surface>& surface);

private:
    /// <summary>
    /// Checks various suitability requirements of the GPU
    /// </summary>
    bool checkDeviceSuitable() const;

    /// <summary>
    /// Gets the indices of where the desired queues are on a physical device
    /// </summary>
	void findQueueFamilies(const std::unique_ptr<Surface>& surface);

    /// <summary>
    /// Checks what capabilites, image formats, and present modes the GPU supports for swapchains
    /// </summary>
    void querySwapchainSupport(const std::unique_ptr<Surface>& surfacePtr);

    /// <summary>
    /// Checks if the GPU supports the extensions required
    /// </summary>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

    VkSampleCountFlagBits getMaxUsableSampleCount() const;
private:
	/// <summary>
	/// Handle to reference the graphics card used by vulkan
	/// </summary>
	VkPhysicalDevice device = VK_NULL_HANDLE;

    QueueFamilyIndices queueFamilyIndices;
    SwapchainSupportDetails swapchainSupportDetails;
    VkSampleCountFlagBits msaaSampleCount;
};