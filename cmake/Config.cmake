# detect the OS

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM_WINDOWS 1)

    message(STATUS "Windows system detected")


    # don't use the OpenGL ES implementation on Windows
    set(OPENGL_ES 0)

    # detect the architecture
    if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "ARM64" OR "${MSVC_CXX_ARCHITECTURE_ID}" MATCHES "ARM64" OR "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "ARM64")
        set(ARCH_ARM64 1)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_X86 1)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_X64 1)
    else()
        message(FATAL_ERROR "Unsupported architecture")
        return()
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLATFORM_UNIX 1)

    set(OPENGL_ES 0)

    message(STATUS "Linux system detected")

    if(ANDROID)
        message(FATAL_ERROR "Unsupported ANDROID system or environment")
    endif()

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
   message(FATAL_ERROR "Unsupported ANDROID system or environment")
else()
    message(FATAL_ERROR "Unsupported operating system or environment")
    return()
endif()

if(MSVC)
    set(COMPILER_MSVC 1)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(COMPILER_CLANG_CL 1)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_CLANG 1)

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE CLANG_COMPILER_VERSION ERROR_VARIABLE CLANG_COMPILER_VERSION)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_GCC 1)

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE GCC_COMPILER_VERSION ERROR_VARIABLE GCC_COMPILER_VERSION)
    string(REGEX MATCHALL ".*(tdm[64]*-[1-9]).*" COMPILER_GCC_TDM "${GCC_COMPILER_VERSION}")
else()
    message(WARNING "Unrecognized compiler: ${CMAKE_CXX_COMPILER_ID}. Use at your own risk.")
endif()