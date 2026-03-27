list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

include(CMakeFindDependencyMacro)

# detect the PLATFORM
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(FIND_PLATFORM_WINDOWS 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(FIND_PLATFORM_LINUX 1)

    if(@USE_DRM@)
        set(FIND_USE_DRM 1)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(FIND_PLATFORM_ANDROID 1)
endif()

# start with an empty list
set(FIND_DEPENDENCIES_NOTFOUND)

if(FIND_USE_DRM)
    find_dependency(DRM)
    find_dependency(GBM)
elseif(FIND_PLATFORM_LINUX)
    find_dependency(X11 REQUIRED COMPONENTS Xrandr XCursor_cls)
endif()

if(FIND_PLATFORM_LINUX)
    find_dependency(UDev)
endif()

if(NOT FIND_PLATFORM_ANDROID)
    if(NOT OpenGL_GL_PREFERENCE)
        set(OpenGL_GL_PREFERENCE "LEGACY")
    endif()
    find_dependency(OpenGL COMPONENTS OpenGL)
endif()

if(FIND_DEPENDENCIES_NOTFOUND)
    set(FIND_ERROR "SLE found but some of its dependencies are missing (${FIND_DEPENDENCIES_NOTFOUND})")
    set(FOUND OFF)
endif()
