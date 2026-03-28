#pragma once
#if _WIN32 || _WIN64
    #define PLATFORM_WINDOWS 1



#elif __linux__

    #define PLATFORM_LINUX 1


#else 

    #error "Unsupported platform"

#endif