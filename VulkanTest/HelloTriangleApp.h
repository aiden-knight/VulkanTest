#pragma once
#include "Structures.h"

class HelloTriangleApp {
public: // PUBLIC FUNCTIONS

    /// <summary>
    /// Initalises, runs, then cleans up the vulkan application
    /// </summary>
    void run();

private: // PRIVATE VARIABLES

    /// <summary>
    /// Pointer to the glfw window in use
    /// </summary>
    GLFWwindow* window = nullptr;
    
    /// <summary>
    /// Used to send messages from validation layers to application assuming they're enabled
    /// </summary>
    VkDebugUtilsMessengerEXT debugMessenger;

    /// <summary>
    /// Connection between application and the vulkan library
    /// </summary>
    VkInstance instance;

    /// <summary>
    /// Handle to reference the graphics card used by vulkan
    /// </summary>
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /// <summary>
    /// Logical device, aka the application's software representaton of the physical device
    /// </summary>
    VkDevice device;

    /// <summary>
    /// A queue on the physical device that supports graphics commands
    /// </summary>
    VkQueue graphicsQueue;

    /// <summary>
    /// A queue on the physical device that supports screen presentation commands
    /// </summary>
    VkQueue presentQueue;

    /// <summary>
    /// abstraction of the window surface to be rendered to, required for on-screen rendering
    /// </summary>
    VkSurfaceKHR surface;

    /// <summary>
    /// Queue of images to render to the screen
    /// Owner of the framebuffers that are rendered to
    /// </summary>
    VkSwapchainKHR swapchain;

    /// <summary>
    /// Vector of the swapchain's images
    /// Each containing the actual data of the images
    /// </summary>
    std::vector<VkImage> swapchainImages;

    /// <summary>
    /// Vector of the swapchain's image views
    /// Each containing data on how to interpret the linked image's data
    /// </summary>
    std::vector<VkImageView> swapchainImageViews;

    /// <summary>
    /// Vector of the swapchain's framebuffers
    /// Each a collection of memory attachments used for rendering to by the render pass
    /// </summary>
    std::vector<VkFramebuffer> swapchainFramebuffers;
    
    /// <summary>
    /// Render pass encapsulates the state needed for renderering to the target, for example: 
    /// what buffers will be in the framebuffer rendered to;
    /// how many samples to use for each buffer;
    /// how the contents of the buffer should be handled during rendering
    /// </summary>
    VkRenderPass renderPass;

    /// <summary>
    /// Describes the uniform values to pass to shaders
    /// </summary>
    VkPipelineLayout pipelineLayout;

    /// <summary>
    /// Representation of the entire graphics pipeline, both fixed function and programmable parts in the form of shaders
    /// </summary>
    VkPipeline graphicsPipeline;

    /// <summary>
    /// The format that images in the swapchain take, aka the way the pixel data is stored (ie 32 bytes RGBA)
    /// and what colour space they are in (ie sRGB)
    /// </summary>
    VkFormat swapchainImageFormat;

    /// <summary>
    /// The width and height of images in the swapchain
    /// </summary>
    VkExtent2D swapchainExtent;

    /// <summary>
    /// Responsible for memory management and allocation of command buffers
    /// </summary>
    VkCommandPool commandPool;
    
    /// <summary>
    /// Whether the window, thus framebuffer, has been resized
    /// </summary>
    bool frameBufferResized = false;

    /// <summary>
    /// The current frame to be rendered by the GPU, will be from [0, MAX_FRAMES_IN_FLIGHT)
    /// </summary>
    uint32_t currentFrame = 0;

    /// <summary>
    /// Buffer of commands to be executed, often cleared and written into
    /// </summary>
    std::vector<VkCommandBuffer> commandBuffers;

    /// <summary>
    /// Semaphore to ensure image has been acquired fully before GPU renders to it
    /// </summary>
    std::vector<VkSemaphore> imageAvailableSemaphores;

    /// <summary>
    /// Semaphore to ensure image has been rendered to before the image gets presented to the screen
    /// </summary>
    std::vector<VkSemaphore> renderFinishedSemaphores;

    /// <summary>
    /// Fence to ensure GPU has finished with the command buffer before the CPU tries to re-use it
    /// </summary>
    std::vector<VkFence> inFlightFences;

private: // PRIVATE FUNCTIONS
    
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

    /// <summary>
    /// For re-use, fills in the messenger create info for sending debug messages to the same callback
    /// </summary>
    void populateDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    /// <summary>
    /// Main loop that runs until glfw says window should close
    /// </summary>
    void mainLoop();

    /// <summary>
    /// Cleans up vulkan objects and then glfw objects
    /// </summary>
    void cleanup();

    // INITIALISATION FUNCTIONS 
    
    /// <summary>
    /// Initialises the glfw window and sets up required glfw callbacks
    /// </summary>
    void initGLFW();

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