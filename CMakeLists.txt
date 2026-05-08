cmake_minimum_required(VERSION 3.10)
project("SLE")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Config.cmake)
include(cmake/Macros.cmake)

find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Vulkan trouvé : ${Vulkan_LIBRARIES}")
else()
    message(FATAL_ERROR "Vulkan non trouvé !")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/Libs/headers/glfw)
include_directories(${PROJECT_SOURCE_DIR}/Libs/headers/glfw/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/Libs/headers/glm)
include_directories(${PROJECT_SOURCE_DIR}/Libs/headers/glm)

add_subdirectory(${PROJECT_SOURCE_DIR}/Libs/headers/stb)
include_directories(${PROJECT_SOURCE_DIR}/Libs/headers/stb)

add_subdirectory(${PROJECT_SOURCE_DIR}/Libs/headers/tinyobjloader)
include_directories(${PROJECT_SOURCE_DIR}/Libs/headers/tinyobjloader)


#find_package (tinyobjloader REQUIRED)


add_subdirectory(src/SLE)

add_subdirectory(src/Game)

# Useful message
message(STATUS "Project: ${PROJECT_NAME}")