#pragma once
#include <vulkan/vulkan.h>
#include "memory"

class LogicalDevice;
class PhysicalDevice;
class CommandPool;

class Buffer {
public:
	Buffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, VkDeviceSize size, VkBufferUsageFlags flags, VkMemoryPropertyFlags properties);
	~Buffer();

	const VkBuffer getBuffer() const { return buffer; }

	void copyFromData(void* inputData);
	void copyFromBuffer(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<Buffer>& source);
	void copyToImage(const std::unique_ptr<CommandPool>& commandPool, VkImage image, uint32_t width, uint32_t height);
	void mapMemory(void** target);
private:
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	const VkDeviceSize size;
	const std::unique_ptr<LogicalDevice>& device;
};

