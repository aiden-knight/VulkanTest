#pragma once
#include "Structures.h"

class HelloTriangleApp {
public: //                         PUBLIC FUNCTIONS

    /// <summary>
    /// Initalises, runs, then cleans up the vulkan application
    /// </summary>
    void run();

private: //                         PRIVATE VARIABLES

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

    VkQueue transferQueue;

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
    /// Describes layout of things to be passed to the shader
    /// </summary>
    VkDescriptorSetLayout descriptorSetLayout;

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
    /// Responsible for temporary transfer command buffers
    /// </summary>
    VkCommandPool transferCommandPool;
    
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

    // can use same buffer for both indices and vertices
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    
    uint32_t mipLevels;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkImage textureImage;
    VkImageView textureImageView;
    VkDeviceMemory textureImageMemory;

    VkImage depthImage;
    VkImageView depthImageView;
    VkDeviceMemory depthImageMemory;
    
    VkImage colorImage;
    VkImageView colorImageView;
    VkDeviceMemory colorImageMemory;

    VkSampler textureSampler;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    QueueFamilyIndices queueFamilyIndices;

    // OBJ
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

private: //                         PRIVATE FUNCTIONS
    
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

    void updateUniformBuffer(uint32_t currentImage);

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

    //                              INITIALISATION FUNCTIONS 
    
    /// <summary>
    /// Initialises the glfw window and sets up required glfw callbacks
    /// </summary>
    void initGLFW();

    // vulkan initialisation functions for creating respective objects
    void createInstance();
    void setupDebugMessenger();
    void createSurface();

    // GPU interfacing
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createCommandPool();

    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createDepthResources();
    void createColorResources();
    void createFramebuffers();

    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();

    // VkBuffer creation
    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();

    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    /// <summary>
    /// Calls all the relevant create functions
    /// </summary>
    void initVulkan();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags flags, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) const;

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSample, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool) const;
    void endSingleTimeCommands(CommandBufferSubmitInfo info) const;
    void endSingleTimeCommandsQueueTransfer(CommandBufferSubmitInfo src, CommandBufferSubmitInfo dst, VkPipelineStageFlags flags) const;

    /// <summary>
    /// Destroys all swapchain specific objects
    /// </summary>
    void cleanupSwapchain();

    /// <summary>
    /// If window has resized, calls cleanupSwapchain() then creates new swapchain with new window size
    /// </summary>
    void recreateSwapchain();

    /// <summary>
    /// Creates and returns a vector of extensions required for the vulkan instance
    /// </summary>
    std::vector<const char*> getRequiredExtensions() const;

    /// <summary>
    /// Gets the indices of where the desired queues are on a physical device
    /// </summary>
    /// <param name="device">The GPU to look on</param>
    /// <returns>A struct containing the indices or value that indicates they weren't found</returns>
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

    /// <summary>
    /// Checks what capabilites, image formats, and present modes the GPU supports for swapchains
    /// </summary>
    /// <param name="device">GPU to check</param>
    /// <returns>Struct containing info wanted</returns>
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;

    /// <summary>
    /// Creates a vulkan shader module from compiled binary code of the shader
    /// </summary>
    VkShaderModule createShaderModule(const std::vector<char>& code) const;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkSampleCountFlagBits getMaxUsableSampleCount();
    VkFormat findDepthFormat();
    bool hasStencilComponent(VkFormat format);

    /// <summary>
    /// Finds supported format with specific tiling and features
    /// </summary>
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    //                              CHOOSING SWAPCHAIN PARAMETERS

    /// <summary>
    /// Chooses the best image format for the swapchain out of those available
    /// </summary>
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;

    /// <summary>
    /// Chooses the best present mode for the swapchain out of those available
    /// </summary>
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

    /// <summary>
    /// Calculates the swapchain extent that best fits framebuffer but still abides by capabilities
    /// </summary>
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

    //                              VALIDATING VULKAN REQUIREMENTS

    /// <summary>
    /// Checks if required instance extensions are supported
    /// </summary>
    /// <param name="extensions">Required extensions</param>
    bool checkSupported(const std::vector<const char*> extensions) const;

    /// <summary>
    /// Checks if the validation layers are supported on the instance
    /// </summary>
    bool checkValidationLayerSupport() const;

    /// <summary>
    /// Checks if the GPU supports the extensions required
    /// </summary>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

    /// <summary>
    /// Checks if the GPU has the required queue families, swapchain support, and extensions support
    /// </summary>
    bool checkDeviceSuitable(VkPhysicalDevice device) const;

    /// <summary>
    /// vulkan debug callback for when validation layers are setup
    /// </summary>
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    /// <summary>
    /// glfw callback for window resized or minimised
    /// </summary>
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    /// <summary>
    /// Reads binary in from a file
    /// </summary>
    /// <param name="filename">path of file to read from</param>
    static std::vector<char> readFile(const std::string& filename);
};