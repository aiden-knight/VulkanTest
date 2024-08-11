#pragma once
#include <vulkan/vulkan.h>

struct Queues {
	VkQueue graphics;
	VkQueue present;
	VkQueue transfer;
};