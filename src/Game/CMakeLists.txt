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


file(GLOB_RECURSE Asset_Models "${CMAKE_SOURCE_DIR}/Assets/Models/*.*")

foreach(Models ${Asset_Models})
    get_filename_component(Models_NAME ${Models} NAME)

    add_custom_command(
        TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${Models}" "${CMAKE_BINARY_DIR}/Assets/Models/${Models_NAME}"
    )
endforeach()

file(GLOB_RECURSE Asset_Textures "${CMAKE_SOURCE_DIR}/Assets/Textures/*.*")

foreach(Texture ${Asset_Textures})
    get_filename_component(Texture_NAME ${Texture} NAME)

    add_custom_command(
        TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${Texture}" "${CMAKE_BINARY_DIR}/Assets/Textures/${Texture_NAME}"
    )
endforeach()

file(GLOB_RECURSE Shader_SPV "${CMAKE_SOURCE_DIR}/Shaders/*.spv")

foreach(SPV ${Shader_SPV})
    get_filename_component(SPV_NAME ${SPV} NAME)

    add_custom_command(
        TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SPV}" "${CMAKE_BINARY_DIR}/Shaders/${SPV_NAME}"
    )
endforeach()
