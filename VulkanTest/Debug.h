#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#ifndef NDEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif // !NDEBUG

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

namespace Debug {
	/// <summary>
	/// vulkan debug callback for when validation layers are setup
	/// </summary>
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void checkVkResult(VkResult err);

	/// <summary>
	/// For re-use, fills in the messenger create info for sending debug messages to the same callback
	/// </summary>
	void populateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void setupDebugMessenger(VkInstance instance);
	void destroyDebugMessenger(VkInstance instance);

	void exception(std::string message);
	void log(std::string message);
}