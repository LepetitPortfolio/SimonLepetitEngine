# include the SFML specific macros
include(${PROJECT_SOURCE_DIR}/cmake/Macros.cmake)

# define the path of our additional CMake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# set the output directory for libraries
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/Lib")

# add the modules subdirectories

# Common 
add_subdirectory(Common)

# main
if(PLATFORM_WINDOWS)
    add_subdirectory(Main)
endif()

# window
if(BUILD_WINDOW OR BUILD_GRAPHICS)
    add_subdirectory(System)
endif()

if(COMPILER_GCC)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("#include <atomic> int main(){std::atomic_ullong x(1); return x.fetch_add(1);}" ATOMIC_TEST)
    if(NOT ATOMIC_TEST)
#        if(BUILD_GRAPHICS)
#            target_link_libraries(graphics PRIVATE atomic)
#        endif()
#        if(BUILD_AUDIO)
#            target_link_libraries(audio PRIVATE atomic)
#        endif()
    endif()
endif()