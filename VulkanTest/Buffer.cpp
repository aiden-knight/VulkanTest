#include "Buffer.h"
#include "Debug.h"

#include "Image.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "CommandPool.h"

Buffer::Buffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, VkDeviceSize size, VkBufferUsageFlags flags, VkMemoryPropertyFlags properties) :
    device(device), size(size) {
    QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();
    uint32_t queueIndices[] = { indices.graphicsFamily.value() };

    // create vertex buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = flags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        Debug::exception("failed to create vertex buffer");
    }

    // allocate vertex buffer memory
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = Image::findMemoryType(memoryRequirements.memoryTypeBits, properties, physicalDevice);

    // @TODO change to use offset as should split up a single allocation among many different objects 
    // due to physical device's maxMemoryAllocationCount
    if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        Debug::exception("failed to allocate vertex buffer memory");
    }

    // bind buffer memory to the buffer
    vkBindBufferMemory(device->getDevice(), buffer, bufferMemory, 0);
}

Buffer::~Buffer() {
    vkDestroyBuffer(device->getDevice(), buffer, nullptr);
    vkFreeMemory(device->getDevice(), bufferMemory, nullptr);
}

void Buffer::copyFromData(void* inputData) {
    void* data;
    vkMapMemory(device->getDevice(), bufferMemory, 0, size, 0, &data);
    std::memcpy(data, inputData, static_cast<size_t>(size));
    vkUnmapMemory(device->getDevice(), bufferMemory);
}

void Buffer::copyFromBuffer(const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool, const std::unique_ptr<Buffer>& source) {
    QueueFamilyIndices indices = physicalDevice->getQueueFamilyIndices();

    VkCommandBuffer commandBuffer = transferPool->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, source->buffer, buffer, 1, &copyRegion);

    // setup barrier for queue transfer @TODO check queues are different
    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = indices.transferFamilyOnly.value();
    barrier.dstQueueFamilyIndex = indices.graphicsFamily.value();
    barrier.buffer = buffer;
    barrier.offset = 0;
    barrier.size = size;

    barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
        0, nullptr,
        1, &barrier,
        0, nullptr);

    VkCommandBuffer destCommandBuffer = graphicsPool->beginSingleTimeCommands();

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkCmdPipelineBarrier(destCommandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0,
        0, nullptr,
        1, &barrier,
        0, nullptr);

    graphicsPool->endSingleTimeCommandsTransfer(transferPool, commandBuffer, destCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT);
}

void Buffer::copyToImage(const std::unique_ptr<CommandPool>& commandPool, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer copyCmdBuffer = commandPool->beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(copyCmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandPool->endSingleTimeCommands(copyCmdBuffer);
}

void Buffer::mapMemory(void** target) {
    vkMapMemory(device->getDevice(), bufferMemory, 0, size, 0, target);
}
