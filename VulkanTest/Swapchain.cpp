#include "Swapchain.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "Debug.h"

#include "Image.h"

Swapchain::Swapchain(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<Surface>& surface, VkExtent2D surfaceExtent, const Swapchain* oldSwapchain) :
    device(device) {
    createHandle(physicalDevice, surface, surfaceExtent, oldSwapchain);
    createImageViews();

    depthFormat = findDepthFormat(physicalDevice);
}

Swapchain::~Swapchain() {
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device->getDevice(), framebuffer, nullptr);
    }

    for (auto imageView : imageViews) {
        vkDestroyImageView(device->getDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(device->getDevice(), swapchain, nullptr);

    vkDestroyImageView(device->getDevice(), depthImageView, nullptr);
    vkDestroyImage(device->getDevice(), depthImage, nullptr);
    vkFreeMemory(device->getDevice(), depthImageMemory, nullptr);

    vkDestroyImageView(device->getDevice(), colorImageView, nullptr);
    vkDestroyImage(device->getDevice(), colorImage, nullptr);
    vkFreeMemory(device->getDevice(), colorImageMemory, nullptr);
}

void Swapchain::createRenderResources(const std::unique_ptr<PhysicalDevice>& physicalDevice, const VkRenderPass renderPass,
                                      const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<CommandPool>& graphicsPool) {
    createDepthResources(physicalDevice, transferPool, graphicsPool);
    createColorResources(physicalDevice);
    createFramebuffers(renderPass);
}

const float Swapchain::getAspectRatio() const {
    return imageExtent.width / (float) imageExtent.height;
}

void Swapchain::createHandle(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<Surface>& surface, VkExtent2D surfaceExtent, const Swapchain* oldSwapchain) {
    const SwapchainSupportDetails supportDetails = physicalDevice->getSwapchainSupportDetails();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(supportDetails.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(supportDetails.presentModes);
    VkExtent2D extent = chooseSwapExtent(supportDetails.capabilities, surfaceExtent);

    uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
    if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount) {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();
    uint32_t queueFamilyIndicesArray[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = supportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    if (oldSwapchain != nullptr) {
        createInfo.oldSwapchain = oldSwapchain->getSwapchain();
    } else {
        createInfo.oldSwapchain = VK_NULL_HANDLE;
    }

    if (vkCreateSwapchainKHR(device->getDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        Debug::exception("failed to create swap chain");
    }

    // get swapchain images from created swapchain
    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device->getDevice(), swapchain, &imageCount, images.data());

    // store swapchain format and extent for later use
    imageFormat = surfaceFormat.format;
    imageExtent = extent;
}

void Swapchain::createImageViews() {
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        imageViews[i] = Image::createImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device);
    }
}

void Swapchain::createDepthResources(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<CommandPool>& graphicsPool) {
    depthImage = Image::createImage(imageExtent.width, imageExtent.height, 1, physicalDevice->getSampleCount(), depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImageMemory, device, physicalDevice);
    depthImageView = Image::createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, device);

    Image::transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, device, physicalDevice, transferPool, graphicsPool.get());
}

void Swapchain::createColorResources(const std::unique_ptr<PhysicalDevice>& physicalDevice) {
    VkFormat colorFormat = imageFormat;

    colorImage = Image::createImage(imageExtent.width, imageExtent.height, 1, physicalDevice->getSampleCount(), colorFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImageMemory, device, physicalDevice);
    colorImageView = Image::createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device);
}

void Swapchain::createFramebuffers(const VkRenderPass renderPass) {
    framebuffers.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            colorImageView,
            depthImageView,
            imageViews[i]
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        createInfo.pAttachments = attachments.data();
        createInfo.width = imageExtent.width;
        createInfo.height = imageExtent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(device->getDevice(), &createInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            Debug::exception("failed to create framebuffer");
        }
    }
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D windowExtent) const {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D extent{
            std::clamp(windowExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(windowExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };

        return extent;
    }
}

VkFormat Swapchain::findDepthFormat(const std::unique_ptr<PhysicalDevice>& physicalDevice) {
    return findSupportedFormat(physicalDevice,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat Swapchain::findSupportedFormat(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice->getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    Debug::exception("failed to find supported format");
    return VK_FORMAT_MAX_ENUM;
}
