#include "PhysicalDevice.h"
#include "Debug.h"
#include "Surface.h"
#include "GraphicsInstance.h"

PhysicalDevice::PhysicalDevice(const std::unique_ptr<GraphicsInstance>& instance, const std::unique_ptr<Surface>& surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        Debug::exception("failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance->getInstance(), &deviceCount, devices.data());

    // find first device that is sufficient for running the program
    for (const auto& device : devices) {
        if (!checkDeviceExtensionSupport(device)) continue; // if device doesn't support extensions we want skip
        
        // set device to one we're checking
        this->device = device;
        findQueueFamilies(surface); // find desired queue families
        querySwapchainSupport(surface); // fill out swapchain support details 

        // if device unsuitable reset to null otherwise get sample count then finish
        if (checkDeviceSuitable()) {
            msaaSampleCount = getMaxUsableSampleCount();
            break;
        } else {
            this->device = VK_NULL_HANDLE;
        }
    }

    if (device == VK_NULL_HANDLE) {
        Debug::exception("failed to find a suitable GPU!");
    }
}

void PhysicalDevice::updateSwapchainSupport(const std::unique_ptr<Surface>& surface) {
    querySwapchainSupport(surface);
}

bool PhysicalDevice::checkDeviceSuitable() const {
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    bool hasFeatureSupport = deviceFeatures.samplerAnisotropy;
    
    bool hasQueueFamilies = queueFamilyIndices.isComplete();
    bool swapchainAdequate = !(swapchainSupportDetails.formats.empty() || swapchainSupportDetails.presentModes.empty());;

    return hasQueueFamilies && swapchainAdequate && hasFeatureSupport;
}

void PhysicalDevice::findQueueFamilies(const std::unique_ptr<Surface>& surface) {
    queueFamilyIndices = QueueFamilyIndices(); // reset back to base

	uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphicsFamily = i;
        } else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            queueFamilyIndices.transferFamilyOnly = i;
        }

        // can queue present to created window surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->getSurface(), &presentSupport);
        if (presentSupport) {
            queueFamilyIndices.presentFamily = i;
        }

        if (queueFamilyIndices.isComplete()) break;
        i++;
    }
}

void PhysicalDevice::querySwapchainSupport(const std::unique_ptr<Surface>& surfacePtr) {
    swapchainSupportDetails = SwapchainSupportDetails(); // reset back to base

    VkSurfaceKHR surface = surfacePtr->getSurface();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainSupportDetails.capabilities);

    uint32_t formatCount = 0, presentModeCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        swapchainSupportDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainSupportDetails.formats.data());
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        swapchainSupportDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapchainSupportDetails.presentModes.data());
    }
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() const {
    VkPhysicalDeviceProperties physicalDeviceProps;
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProps);

    VkSampleCountFlags counts = physicalDeviceProps.limits.framebufferColorSampleCounts & physicalDeviceProps.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}
