#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class RenderingApp
{
public:
	void run();

private:
	// Init glfw window
	void initWindow();
	// VK instance
	void createInstance();
	void initVulkan();
	void setupDebugMessenger();
	void pickPhysicalDevice();

	void mainLoop();
	void cleanup();

	void printSupportedExtensions() const;
	bool checkValidationLayerSupport() const;
	std::vector<const char*> getRequiredExtensions() const;

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	GLFWwindow* m_window = nullptr;
	unsigned int m_width = 800;
	unsigned int m_height = 600;
};