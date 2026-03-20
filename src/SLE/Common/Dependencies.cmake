list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

include(CMakeFindDependencyMacro)

# start with an empty list
set(FIND_DEPENDENCIES_NOTFOUND)

find_dependency(Threads)

if(FIND_DEPENDENCIES_NOTFOUND)
    set(FIND_ERROR "SLE found but some of its dependencies are missing (${FIND_DEPENDENCIES_NOTFOUND})")
    set(FOUND OFF)
endif()
