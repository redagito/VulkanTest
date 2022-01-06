#include "RenderingApp.h"

#include <iostream>
#include <vector>
#include <cstring>

#ifndef NDEBUG
    static const bool enableValidationLayers = true;
#else
    static const bool enableValidationLayers = false;
#endif

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

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
    std::uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = nullptr;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Validation layers
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create vk instance"};
    }
}

void RenderingApp::initVulkan()
{
    createInstance();
    printSupportedExtensions();
    checkValidationLayerSupport();
}

void RenderingApp::cleanup()
{
    vkDestroyInstance(m_instance, nullptr);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}