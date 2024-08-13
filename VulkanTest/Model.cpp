#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Buffer.h"
#include "Debug.h"

Model::Model(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool, std::string path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        Debug::exception(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    createVertexBuffer(device, physicalDevice, graphicsPool, transferPool);
    createIndexBuffer(device, physicalDevice, graphicsPool, transferPool);
}

void Model::draw(VkCommandBuffer cmdBuffer) {
    const std::array<VkDeviceSize, 1> offsets = { 0 };
    const std::array<VkBuffer, 1> buffers = { vertexBuffer->getBuffer() };
    vkCmdBindVertexBuffers(cmdBuffer, 0, static_cast<uint32_t>(buffers.size()), buffers.data(), offsets.data());
    vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Draw command for the triangle
    vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Model::createVertexBuffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool) {
    VkDeviceSize size = sizeof(vertices[0]) * vertices.size();

    auto stagingBuffer = std::make_unique<Buffer>(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    stagingBuffer->copyFromData(vertices.data());

    vertexBuffer = std::make_unique<Buffer>(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vertexBuffer->copyFromBuffer(physicalDevice, graphicsPool, transferPool, stagingBuffer);
}

void Model::createIndexBuffer(const std::unique_ptr<LogicalDevice>& device, const std::unique_ptr<PhysicalDevice>& physicalDevice, const std::unique_ptr<CommandPool>& graphicsPool, const std::unique_ptr<CommandPool>& transferPool) {
    VkDeviceSize size = sizeof(indices[0]) * indices.size();

    auto stagingBuffer = std::make_unique<Buffer>(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    stagingBuffer->copyFromData(indices.data());

    indexBuffer = std::make_unique<Buffer>(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    indexBuffer->copyFromBuffer(physicalDevice, graphicsPool, transferPool, stagingBuffer);
}
