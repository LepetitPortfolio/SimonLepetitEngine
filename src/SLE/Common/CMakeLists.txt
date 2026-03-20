message(STATUS "sle-common - CMkeList")

set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SLE/Common)

file(GLOB_RECURSE PROJECT_SOURCES
  "${SRCROOT}/*.cpp"
  "${SRCROOT}/*.cxx"
  "${SRCROOT}/*.cc"
)

# Also add headers to the IDE view (optional)
file(GLOB_RECURSE PROJECT_HEADERS
  "${SRCROOT}/*.h"
  "${SRCROOT}/*.hpp"
)

source_group("" FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})

if(PLATFORM_WINDOWS)
    set(PLATFORM_SRC
        #${SRCROOT}/Win32/SleepImpl.cpp
        #${SRCROOT}/Win32/SleepImpl.hpp
    )
    source_group("windows" FILES ${PLATFORM_SRC})
else()
    set(PLATFORM_SRC
        #${SRCROOT}/Unix/SleepImpl.cpp
        #${SRCROOT}/Unix/SleepImpl.hpp
    )

    source_group("unix" FILES ${PLATFORM_SRC})
endif()

find_package(Threads REQUIRED)

#message(WARNING "Threads found: ${Threads_FOUND}")

AddLibrary(Common SOURCES ${PROJECT_SOURCES} ${PLATFORM_SRC} DEPENDENCIES "Dependencies.cmake")

if (ENABLE_PCH)
    message(VERBOSE "enabling PCH for SLE library 'sle-common' (reused as the PCH for other SLE libraries)")
    target_precompile_headers(sle-common PRIVATE ${PROJECT_SOURCE_DIR}/src/SLE/PCH.hpp)
endif()

if(PLATFORM_ANDROID)
    target_include_directories(sle-common SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/Libs/headers/glad/include")
endif()

target_link_libraries(sle-common PRIVATE Threads::Threads)

if(PLATFORM_LINUX)
    target_link_libraries(common PRIVATE rt)
elseif(PLATFORM_WINDOWS)
    target_link_libraries(sle-common PRIVATE winmm)
elseif(PLATFORM_ANDROID)
    target_link_libraries(sle-common PRIVATE android log)
endif()
