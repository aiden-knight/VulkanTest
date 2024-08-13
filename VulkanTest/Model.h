#pragma once
#include "ModelData.h"
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>

class Buffer;
class CommandPool;
class LogicalDevice;
class PhysicalDevice;

// @TODO NEED DESCRIPTOR SET PER MODEL
class Model {
public:
	Model(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool, std::string path);

	void draw(VkCommandBuffer cmdBuffer);

private:
	void createVertexBuffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool);
	void createIndexBuffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool);

private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
};

