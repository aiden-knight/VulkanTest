#include "LogicalDevice.h"
#include "PhysicalDevice.h"

#include "Debug.h"

LogicalDevice::LogicalDevice(const std::unique_ptr<PhysicalDevice>& physicalDevice) {
    QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();

    // DEVICE QUEUES
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamilyOnly.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }


    // DEVICE FEATURES
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // LOGICAL DEVICE
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // only required to support older versions of Vulkan
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // CREATE LOGICAL DEVICE
    if (vkCreateDevice(physicalDevice->getPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
        Debug::exception("failed to create logical device");
    }
}

LogicalDevice::~LogicalDevice() {
	vkDestroyDevice(device, nullptr);
}

Queues LogicalDevice::getQueueHandles(const std::unique_ptr<PhysicalDevice>& physicalDevice) const {
    QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();
    Queues queues{};

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &queues.graphics);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &queues.present);
    vkGetDeviceQueue(device, indices.transferFamilyOnly.value(), 0, &queues.transfer);

    return queues;
}
