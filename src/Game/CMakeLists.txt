message(STATUS "Game - CMkeList")

set(GAMESRCROOT ${PROJECT_SOURCE_DIR}/src/Game)

file(GLOB_RECURSE GAME_SOURCES
  "${GAMESRCROOT}/*.cpp"
  "${GAMESRCROOT}/*.cxx"
  "${GAMESRCROOT}/*.cc"
)

# Also add headers to the IDE view (optional)
file(GLOB_RECURSE GAME_HEADERS
  "${GAMESRCROOT}/*.h"
  "${GAMESRCROOT}/*.hpp"
)

add_executable(Game ${GAME_SOURCES} ${GAME_HEADERS})

target_link_libraries(Game PRIVATE SLE)

target_include_directories(Game PRIVATE ${CMAKE_SOURCE_DIR}/src/SLE)

target_link_libraries(Game PRIVATE SLE Vulkan::Vulkan)
