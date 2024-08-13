#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <string>
#include <cmath>

class PhysicalDevice;
class LogicalDevice;
class CommandPool;

class Texture {
public:
	Texture(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool, const std::string path);
	~Texture();

	const uint32_t getMipLevels() const { return mipLevels; }
	const VkImageView getImageView() const { return imageView; }

	static constexpr VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	static void setFormatProperties(VkFormatProperties properties) { formatProperties = properties; }

private:
	void generateMipmaps(const std::unique_ptr<CommandPool>& graphicsPool, VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

private:
	uint32_t mipLevels;
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;

	const std::unique_ptr<LogicalDevice>& device;
	static VkFormatProperties formatProperties;
};