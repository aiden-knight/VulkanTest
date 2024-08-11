#include "GraphicsInstance.h"
#include "Window.h"
#include "Debug.h"

GraphicsInstance::GraphicsInstance(const std::unique_ptr<Window>& window) {
    // APPLICATION INFO
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // INSTANCE CREATE INFO
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // get extensions required by glfw
    std::vector<const char*> extensions = getRequiredExtensions(window);

    if (!checkSupported(extensions)) {
        Debug::exception("required instance extensions not supported");
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());;
    createInfo.ppEnabledExtensionNames = extensions.data();

    // validation layers
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        Debug::exception("validation layers requested, but not available!");
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        Debug::populateDebugUtilsMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // CREATE INSTANCE
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        Debug::exception("failed to create instance");
    }

    Debug::setupDebugMessenger(instance);
}

GraphicsInstance::~GraphicsInstance() {
    Debug::destroyDebugMessenger(instance);
    vkDestroyInstance(instance, nullptr);
}

bool GraphicsInstance::checkSupported(const std::vector<const char*> extensions) const {
    // get supported instance extensions
    uint32_t supportedExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

    std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

    // loop over wanted extensions to check if they are available
    for (const char* wanted : extensions) {
        auto isInVector = [wanted](const VkExtensionProperties& prop) { return strcmp(prop.extensionName, wanted) == 0; };
        if (std::find_if(supportedExtensions.begin(), supportedExtensions.end(), isInVector) == supportedExtensions.end()) {
            return false;
        }
    }
    return true;
}

std::vector<const char*> GraphicsInstance::getRequiredExtensions(const std::unique_ptr<Window>& window) const {
    std::vector<const char*> extensions = window->getRequiredInstanceExtensions();

    // add extra extension in form of debug utils if built in NDEBUG
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool GraphicsInstance::checkValidationLayerSupport() const {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        auto isInVector = [layerName](const VkLayerProperties& prop) { return strcmp(prop.layerName, layerName) == 0; };
        if (std::find_if(availableLayers.begin(), availableLayers.end(), isInVector) == availableLayers.end()) {
            return false;
        }
    }

    return true;
}