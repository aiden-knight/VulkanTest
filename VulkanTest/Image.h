#pragma once
#include <vulkan/vulkan.h>
#include <memory>

class LogicalDevice;
class PhysicalDevice;
class CommandPool;

class Image {
public:
	static VkImage createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSample, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& imageMemory, const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice);

	static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, const std::unique_ptr<LogicalDevice>& device);

	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice,
									  const std::unique_ptr<CommandPool>& transferPool, const CommandPool* destQueuePool = nullptr);

	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const std::unique_ptr<PhysicalDevice>& physicalDevice);
	static bool hasStencilComponent(VkFormat format);
};