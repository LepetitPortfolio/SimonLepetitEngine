#pragma once

//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>

#include "System/VulkanPlatform.h"

#include <Common/FileReader.h>


int main() 
{
    VulkanPlatform app;

    try 
    {
        app.Run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
