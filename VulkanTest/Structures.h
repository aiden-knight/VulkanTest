#pragma once
// IMGUI
#include "imgui.h"
#include "imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// SINGLE INCLUDES
#include <stb_image.h>
#include <tiny_obj_loader.h>

// STANDARD LIBRARY
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <set>
#include <optional>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <cstring>

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct CommandBufferSubmitInfo {
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkQueue queueHandle = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDesc{};

        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDesc;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDesc{};

        attributeDesc[0].binding = 0;
        attributeDesc[0].location = 0;
        attributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDesc[0].offset = offsetof(Vertex, pos);

        attributeDesc[1].binding = 0;
        attributeDesc[1].location = 1;
        attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDesc[1].offset = offsetof(Vertex, color);

        attributeDesc[2].binding = 0;
        attributeDesc[2].location = 2;
        attributeDesc[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDesc[2].offset = offsetof(Vertex, texCoord);

        return attributeDesc;
    }
    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

// for map
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

const std::vector<Vertex> verticesOLD = {
    // SQUARE ONE
    {{ -0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    // SQUARE TWO
    { { -0.5f, -0.5f, -0.5f }, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
    {{ 0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indicesOLD = {
    0, 1, 2,
    1, 3, 2,
    4, 5, 6,
    5, 7, 6
};