#include <iostream>
#include <cstdlib>

#include "RenderingApp.h"

int main(int argc, char** argv)
{
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