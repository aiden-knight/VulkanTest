#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include "Queues.h"

class PhysicalDevice;

class LogicalDevice {
public:
	LogicalDevice(const std::unique_ptr<PhysicalDevice>& physicalDevice);
	~LogicalDevice();

	const VkDevice getDevice() const { return device; }
	Queues getQueueHandles(const std::unique_ptr<PhysicalDevice>& physicalDevice) const;
private:
	/// <summary>
	/// Logical device, aka the application's software representaton of the physical device
	/// </summary>
	VkDevice device;
};