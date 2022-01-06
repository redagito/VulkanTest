#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

	void mainLoop();
	void cleanup();

	void printSupportedExtensions() const;
	bool checkValidationLayerSupport() const;

	VkInstance m_instance;
	GLFWwindow* m_window = nullptr;
	unsigned int m_width = 800;
	unsigned int m_height = 600;
};