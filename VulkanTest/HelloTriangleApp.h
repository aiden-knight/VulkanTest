#pragma once
#include <memory>
#include "Queues.h"
#include "Structures.h"

class GraphicsInstance;
class Window;
class Surface;
class PhysicalDevice;
class LogicalDevice;
class Swapchain;
class CommandPool;
class Texture;
class Buffer;

class HelloTriangleApp {
public: //                         PUBLIC FUNCTIONS
    HelloTriangleApp();
    ~HelloTriangleApp();
    
    void run();
private: //                         PRIVATE VARIABLES
    std::unique_ptr<GraphicsInstance> instance;

    std::unique_ptr<Window> window;
    std::unique_ptr<Surface> surface;

    std::unique_ptr<PhysicalDevice> physicalDevice;
    std::unique_ptr<LogicalDevice> device;
    Queues queues;

    /// <summary>
    /// Responsible for temporary transfer command buffers
    /// </summary>
    std::unique_ptr<CommandPool> transferCommandPool;
    std::unique_ptr<CommandPool> commandPool;

    /// <summary>
    /// Buffer of commands to be executed, often cleared and written into
    /// </summary>
    std::vector<VkCommandBuffer> commandBuffers;
    
    std::unique_ptr<Swapchain> swapchain;

    std::unique_ptr<Texture> texture;

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
    /// The current frame to be rendered by the GPU, will be from [0, MAX_FRAMES_IN_FLIGHT)
    /// </summary>
    uint32_t currentFrame = 0;


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
    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;

    VkSampler textureSampler;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;

    // OBJ
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // IMGUI
    VkDescriptorPool imguiDescriptorPool;
    ImGuiIO* io;

    ImVec4 clearColor = ImVec4(0.0f, 0.00f, 0.00f, 1.00f);
    bool showDemoWindow = false;
    bool renderStatic = false;
    VkDescriptorSet texDS;

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

    void drawImgui();

    /// <summary>
    /// Cleans up vulkan objects and then glfw objects
    /// </summary>
    void cleanup();

    void cleanupImgui();
    void cleanupVulkan();

    //                              INITIALISATION FUNCTIONS 

    /// <summary>
    /// Initialises ImGui
    /// </summary>
    void initIMGUI();

    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();

    void createTextureSampler();

    // VkBuffer creation
    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();

    void createDescriptorPool();
    void createDescriptorSets();
    void createSyncObjects();

    /// <summary>
    /// Calls all the relevant create functions
    /// </summary>
    void initVulkan();

    /// <summary>
    /// If window has resized, calls cleanupSwapchain() then creates new swapchain with new window size
    /// </summary>
    void recreateSwapchain();

    /// <summary>
    /// Creates a vulkan shader module from compiled binary code of the shader
    /// </summary>
    VkShaderModule createShaderModule(const std::vector<char>& code) const;

    //                              VALIDATING VULKAN REQUIREMENTS

    static void checkVkResult(VkResult err);

    /// <summary>
    /// Reads binary in from a file
    /// </summary>
    /// <param name="filename">path of file to read from</param>
    static std::vector<char> readFile(const std::string& filename);
};