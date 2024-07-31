#pragma once
#include "Structures.h"

class HelloTriangleApp {
public:
    void run();

private:
    GLFWwindow* window = nullptr;
    
    VkDebugUtilsMessengerEXT debugMessenger;

    VkInstance instance;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    VkCommandPool commandPool;
    
    bool frameBufferResized = false;
    uint32_t currentFrame = 0;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

private:
    // RENDERING
    
    /// <summary>
    /// Writes the graphics queue commands we wanted to execute to the command buffer
    /// </summary>
    /// <param name="commandBuffer">The command buffer to write to</param>
    /// <param name="imageIndex">Index of the swapchain image to write to</param>
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    /// <summary>
    /// Renders a frame than adds it to the present queue to be presented to screen
    /// </summary>
    void drawFrame();

    void populateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void mainLoop();
    void cleanup();

    // INITIALISATION
    // glfw

    void initWindow();

    // vulkan

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    void initVulkan();

    void cleanupSwapchain();
    void recreateSwapchain();

    std::vector<const char*> getRequiredExtensions() const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;
    VkShaderModule createShaderModule(const std::vector<char>& code) const;

    // CHOOSING SWAPCHAIN PARAMETERS

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

    // VALIDATING VULKAN REQUIREMENTS

    bool checkSupported(const std::vector<const char*> extensions) const;
    bool checkValidationLayerSupport() const;
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
    bool checkDeviceSuitable(VkPhysicalDevice device) const;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    static std::vector<char> readFile(const std::string& filename);
};