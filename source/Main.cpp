#include <iostream>
#include <cstdlib>

#include "RenderingApp.h"

int main(int argc, char** argv)
{
    std::cout << std::getenv("VULKAN_SDK") << std::endl;
    RenderingApp app;
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}