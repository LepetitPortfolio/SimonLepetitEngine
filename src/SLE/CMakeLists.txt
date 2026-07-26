message(STATUS "SLE - CMkeList")

find_package(Threads REQUIRED)

set(SLESRCROOT ${PROJECT_SOURCE_DIR}/src/SLE)

file(GLOB_RECURSE SLE_SOURCES
  "${SLESRCROOT}/*.cpp"
  "${SLESRCROOT}/*.cxx"
  "${SLESRCROOT}/*.cc"
)

# Also add headers to the IDE view (optional)
file(GLOB_RECURSE SLE_HEADERS
  "${SLESRCROOT}/*.h"
  "${SLESRCROOT}/*.hpp"
)

add_library(SLE STATIC ${SLE_SOURCES} "GameplayConcepts/GameObject/GameObject.cpp" "Core/AssetData.h")


target_link_libraries(SLE PRIVATE Threads::Threads)
target_include_directories(SLE PRIVATE ${Vulkan_INCLUDE_DIRS})

#message(STATUS "Vulkan - ${Vulkan_INCLUDE_DIRS}")


target_compile_definitions(SLE PRIVATE STB_IMAGE_IMPLEMENTATION)

target_compile_definitions(SLE PRIVATE TINYOBJLOADER_IMPLEMENTATION)

target_link_libraries(SLE PRIVATE glfw Vulkan::Vulkan)

#target_link_libraries(SLE PRIVATE spirv-reflect)
target_link_libraries(SLE PRIVATE spirv-reflect-static)



if(Vulkan_FOUND)
    get_filename_component(Vulkan_SDK ${Vulkan_INCLUDE_DIRS} DIRECTORY)

    file(GLOB_RECURSE VULKAN_DLLS "${Vulkan_SDK}/Bin/*.dll")

    foreach(DLL ${VULKAN_DLLS})
        get_filename_component(DLL_NAME ${DLL} NAME)

        add_custom_command(
            TARGET SLE POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "$<TARGET_FILE_DIR:SLE>/${DLL_NAME}"
        )
    endforeach()
endif()

