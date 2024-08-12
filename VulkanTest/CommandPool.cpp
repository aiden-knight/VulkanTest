#include "CommandPool.h"

#include "LogicalDevice.h"
#include "Debug.h"

CommandPool::CommandPool(const std::unique_ptr<LogicalDevice>& device, const VkQueue queueHandle, const uint32_t queueIndex, const VkCommandPoolCreateFlags flags) : 
    device(device),
    queueHandle(queueHandle) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = flags;
    poolInfo.queueFamilyIndex = queueIndex;

    if (vkCreateCommandPool(device->getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        Debug::exception("failed to create command pool");
    }
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(device->getDevice(), commandPool, nullptr);
}

VkCommandBuffer CommandPool::beginSingleTimeCommands() const {
    // create the temporary command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void CommandPool::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
    vkEndCommandBuffer(commandBuffer);

    // submit commands
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queueHandle, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queueHandle); // use fence for multiple transfers

    vkFreeCommandBuffers(device->getDevice(), commandPool, 1, &commandBuffer);
}

void CommandPool::endSingleTimeCommandsTransfer(const std::unique_ptr<CommandPool>& sourcePool, VkCommandBuffer sourceBuffer, VkCommandBuffer destBuffer, VkPipelineStageFlags flags) const {
    vkEndCommandBuffer(sourceBuffer);
    vkEndCommandBuffer(destBuffer);

    // create the semaphore so they wait in the correct places
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device->getDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
        Debug::exception("failed to create semaphore");
    }

    // submit commands
    VkSubmitInfo submitInfoSrc{}, submitInfoDst{};
    submitInfoSrc.sType = submitInfoDst.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfoSrc.commandBufferCount = submitInfoDst.commandBufferCount = 1;
    submitInfoSrc.pCommandBuffers = &sourceBuffer;
    submitInfoDst.pCommandBuffers = &destBuffer;

    submitInfoSrc.signalSemaphoreCount = 1;
    submitInfoSrc.pSignalSemaphores = &semaphore;

    submitInfoDst.waitSemaphoreCount = 1;
    submitInfoDst.pWaitSemaphores = &semaphore;
    submitInfoDst.pWaitDstStageMask = &flags;

    vkQueueSubmit(sourcePool->getQueueHandle(), 1, &submitInfoSrc, VK_NULL_HANDLE);
    vkQueueSubmit(queueHandle, 1, &submitInfoDst, VK_NULL_HANDLE);

    vkQueueWaitIdle(queueHandle); // use fence for multiple transfers
    vkFreeCommandBuffers(device->getDevice(), sourcePool->getCommandPool(), 1, &sourceBuffer);
    vkFreeCommandBuffers(device->getDevice(), commandPool, 1, &destBuffer);
    vkDestroySemaphore(device->getDevice(), semaphore, nullptr);
}

std::vector<VkCommandBuffer> CommandPool::createCommandBuffers(const int amount) const {
    std::vector<VkCommandBuffer> commandBuffers(amount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        Debug::exception("failed to allocate command buffer(s)");
    }
    return commandBuffers;
}
