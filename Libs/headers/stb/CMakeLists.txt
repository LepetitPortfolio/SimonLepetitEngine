message(STATUS "STB - CMkeList")

set(SLESRCROOT ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SLE_SOURCES
  "${SLESRCROOT}/*.cpp"
  "${SLESRCROOT}/*.cxx"
  "${SLESRCROOT}/*.cc"
)

file(GLOB_RECURSE SLE_HEADERS
  "${SLESRCROOT}/*.h"
  "${SLESRCROOT}/*.hpp"
)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories(stb INTERFACE ${SLE_HEADERS})