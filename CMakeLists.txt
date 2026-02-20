# CMakeList.txt : CMake project for SimonLepetitEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(SimonLepetitEngine LANGUAGES CXX)


# Gather all sources under src/ (keeps CMakeLists small; configure-time globbing)
file(GLOB_RECURSE PROJECT_SOURCES
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_SOURCE_DIR}/src/*.cc"
)

# Also add headers to the IDE view (optional)
file(GLOB_RECURSE PROJECT_HEADERS
  "${CMAKE_SOURCE_DIR}/src/*.h"
  "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

# Fail early if no sources found
if(PROJECT_SOURCES STREQUAL "")
  message(FATAL_ERROR "No source files found in ${CMAKE_SOURCE_DIR}/src. Add .cpp files or update glob.")
endif()

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Make sure the compiler uses C++17 for this target (works with multi-config generators)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Include src directory so that includes like "FunctionUtility/FunctionUtility.h" work
#target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")

# Recommended: keep symbol files next to the binary for MSVC multi-config (optional)
if(MSVC)
  foreach(config IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(${PROJECT_NAME} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_BINARY_DIR}/bin
    )
  endforeach()
endif()

# Useful message
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Found ${PROJECT_SOURCES} source files")
message(STATUS "SFML available: ${SFML_FOUND}")
