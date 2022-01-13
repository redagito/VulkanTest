#include "RenderingApp.h"

#include <iostream>
#include <cstring>

#ifndef NDEBUG
    static const bool enableValidationLayers = false;
#else
    static const bool enableValidationLayers = false;
#endif

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
    const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == nullptr) return VK_ERROR_EXTENSION_NOT_PRESENT;

    return func(instance, createInfo, allocator, debugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func == nullptr) return;

    func(instance, debugMessenger, allocator);
}

// Debug callback for validation layer events
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, // Severity
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        std::cerr << "Error: ";
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "Warning: ";
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        std::cerr << "Info: ";
    }
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        std::cerr << "Verbose: ";
    }
    else
    {
        std::cerr << "Unknown: ";
    }

    if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) != 0)
    {
        std::cerr << "General: ";
    }

    if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) != 0)
    {
        std::cerr << "Validation: ";
    }

    if ((messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) != 0)
    {
        std::cerr << "Performance: ";
    }

    std::cerr << "Validation layer: " << callbackData->pMessage << std::endl;

    return VK_FALSE;
}

void RenderingApp::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void RenderingApp::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void RenderingApp::initWindow()
{
    glfwInit();

    // No context for vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Disable resize
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, "Vulkan Test", nullptr, nullptr);
}

void RenderingApp::printSupportedExtensions() const
{
    std::uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions{ extensionCount };
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << extensionCount << " extensions supported\n";
    for (const auto& extension : extensions)
    {
        std::cout << "  " << extension.extensionName << "\n";
    }
}

bool RenderingApp::checkValidationLayerSupport() const
{
    std::uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers{ layerCount };
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    // Check if all our validations layers are supported
    for (auto requiredLayer : validationLayers)
    {
        bool found = false;
        for (const auto& layer : layers)
        {
            if (std::strcmp(layer.layerName, requiredLayer) == 0)
            {
                found = true;
                break;
            }
        }
        // Layer not available
        if (!found) 
        {
            std::cout << "Missing validation layer: " << requiredLayer << "\n";
            return false;
        }
    }
    return true;
}

std::vector<const char*> RenderingApp::getRequiredExtensions() const
{
    std::uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = nullptr;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void RenderingApp::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error{"Validation layer not available"};
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Required extensions
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Validation layers
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<std::uint32_t>(validationLayers.size());
        createInfo.ppEnabledExtensionNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create vk instance"};
    }
}

void RenderingApp::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    printSupportedExtensions();
    pickPhysicalDevice();
}

void RenderingApp::setupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | // Maybe remove
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional data

    if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error{ "Failed to setup debug messenger" };
    }
}

void RenderingApp::pickPhysicalDevice()
{
}

void RenderingApp::cleanup()
{
    if (enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}