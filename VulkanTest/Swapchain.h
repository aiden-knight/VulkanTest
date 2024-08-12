#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <array>
#include <algorithm>

class LogicalDevice;
class PhysicalDevice;
class Surface;
class CommandPool;

class Swapchain {
public:
    Swapchain(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<Surface>& surface, VkExtent2D surfaceExtent, const Swapchain* oldSwapchain = nullptr);
    ~Swapchain();

    void createRenderResources(const std::unique_ptr<PhysicalDevice>& physicalDevice, const VkRenderPass renderPass, 
                               const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<CommandPool>& graphicsPool);

    const VkSwapchainKHR getSwapchain() const { return swapchain; }
    const float getAspectRatio() const;
    const VkFormat getFormat() const { return imageFormat; }
    const VkFormat getDepthFormat() const { return depthFormat; }
    const VkExtent2D getExtent() const { return imageExtent; }
    const VkFramebuffer getFrameBuffer(uint32_t index) { return framebuffers[index]; }
private:
    void createHandle(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<Surface>& surface, VkExtent2D surfaceExtent, const Swapchain* oldSwapchain);
    void createImageViews();

    void createDepthResources(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<CommandPool>& graphicsPool);
    void createColorResources(const std::unique_ptr<PhysicalDevice>& physicalDevice);
    void createFramebuffers(const VkRenderPass renderPass);

    /// <summary>
    /// Chooses the best image format for the swapchain out of those available
    /// </summary>
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

    /// <summary>
    /// Chooses the best present mode for the swapchain out of those available
    /// </summary>
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

    /// <summary>
    /// Calculates the swapchain extent that best fits framebuffer but still abides by capabilities
    /// </summary>
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D windowExtent) const;

    VkFormat findDepthFormat(const std::unique_ptr<PhysicalDevice>& physicalDevice);

    /// <summary>
    /// Finds supported format with specific tiling and features
    /// </summary>
    VkFormat findSupportedFormat(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
private:
    /// <summary>
    /// Queue of images to render to the screen
    /// Owner of the framebuffers that are rendered to
    /// </summary>
    VkSwapchainKHR swapchain;

    /// <summary>
    /// Vector of the swapchain's images
    /// Each containing the actual data of the images
    /// </summary>
    std::vector<VkImage> images;

    /// <summary>
    /// Vector of the swapchain's image views
    /// Each containing data on how to interpret the linked image's data
    /// </summary>
    std::vector<VkImageView> imageViews;

    /// <summary>
    /// Vector of the swapchain's framebuffers
    /// Each a collection of memory attachments used for rendering to by the render pass
    /// </summary>
    std::vector<VkFramebuffer> framebuffers;

    /// <summary>
    /// The format that images in the swapchain take, aka the way the pixel data is stored (ie 32 bytes RGBA)
    /// and what colour space they are in (ie sRGB)
    /// </summary>
    VkFormat imageFormat;

    /// <summary>
    /// The width and height of images in the swapchain
    /// </summary>
    VkExtent2D imageExtent;

    // extra attachments
    VkImage depthImage;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;
    VkFormat depthFormat;

    VkImage colorImage;
    VkImageView colorImageView;
    VkDeviceMemory colorImageMemory;

    const std::unique_ptr<LogicalDevice>& device;
};

