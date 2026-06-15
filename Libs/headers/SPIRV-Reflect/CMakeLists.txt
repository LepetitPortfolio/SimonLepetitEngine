cmake_minimum_required(VERSION 3.16)

project(spirv-reflect)

OPTION(SPIRV_REFLECT_STATIC_LIB     "Build a SPIRV-Reflect static library" OFF)

OPTION(SPIRV_REFLECT_EXECUTABLE     "Build spirv-reflect executable" OFF)

OPTION(SPIRV_REFLECT_STATIC_LIB     "Build a SPIRV-Reflect static library" OFF)
OPTION(SPIRV_REFLECT_BUILD_TESTS    "Build the SPIRV-Reflect test suite" OFF)
OPTION(SPIRV_REFLECT_ENABLE_ASSERTS "Enable asserts for debugging" OFF)
OPTION(SPIRV_REFLECT_ENABLE_ASAN    "Use address sanitization" OFF)
OPTION(SPIRV_REFLECT_INSTALL        "Whether to install" OFF)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")


add_library(spirv-reflect-static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect.h
                                 ${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect.c)

target_include_directories(spirv-reflect-static
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(spirv-reflect-static PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/spirv_reflect.h")

if(SPIRV_REFLECT_INSTALL)
    install(TARGETS spirv-reflect-static
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            PUBLIC_HEADER DESTINATION include)
endif()
