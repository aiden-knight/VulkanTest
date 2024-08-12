#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class LogicalDevice;

class CommandPool {
public:
    CommandPool(const std::unique_ptr<LogicalDevice>& device, const VkQueue queueHandle, const uint32_t queueIndex, const VkCommandPoolCreateFlags flags);
    ~CommandPool();

    const VkCommandPool getCommandPool() const { return commandPool; }
    const VkQueue getQueueHandle() const { return queueHandle; }

    VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    void endSingleTimeCommandsTransfer(const std::unique_ptr<CommandPool>& sourcePool, VkCommandBuffer sourceBuffer, VkCommandBuffer destBuffer, VkPipelineStageFlags flags) const;

    std::vector<VkCommandBuffer> createCommandBuffers(const int amount) const;

private:
    /// <summary>
    /// Responsible for memory management and allocation of command buffers
    /// </summary>
    VkCommandPool commandPool;

    const VkQueue queueHandle;
    const std::unique_ptr<LogicalDevice>& device;
};