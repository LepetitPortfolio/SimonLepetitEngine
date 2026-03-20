include(CMakeParseArguments)

include(${CMAKE_CURRENT_LIST_DIR}/CompilerWarnings.cmake)

function(SetPublicSymbolsHidden target)
    set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
endfunction()

function(SetSTDLib target)
    # for gcc on Windows, apply the USE_STATIC_STD_LIBS option if it is enabled
    if(PLATFORM_WINDOWS)
        if(COMPILER_GCC)
            if(USE_STATIC_STD_LIBS AND NOT COMPILER_GCC_TDM)
                target_link_libraries(${target} PRIVATE "-static-libgcc" "-static-libstdc++")
            elseif(NOT USE_STATIC_STD_LIBS AND COMPILER_GCC_TDM)
                target_link_libraries(${target} PRIVATE "-shared-libgcc" "-shared-libstdc++")
            endif()
        elseif(COMPILER_MSVC)
            if(USE_STATIC_STD_LIBS)
                set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            endif()
        endif()
    endif()
endfunction()

function(ExportTargets)
    
    set(CURRENT_DIR "${PROJECT_SOURCE_DIR}/cmake")

    include(CMakePackageConfigHelpers) 
    
    #write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/SLEConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)

    if(BUILD_FRAMEWORKS)
        set(config_package_location "SLE.framework/Resources/CMake")
    else()
        set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/SLE)
    endif()

    configure_package_config_file("${CURRENT_DIR}/SLEConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/SLEConfig.cmake" INSTALL_DESTINATION "${config_package_location}")

    if(BUILD_SHARED_LIBS)
        set(config_name "Shared")
    else()
        set(config_name "Static")
    endif()
    get_property(ADD_LIBRARY_MODULES GLOBAL PROPERTY ADD_LIBRARY_MODULES_PROPERTY)
    foreach(module ${ADD_LIBRARY_MODULES})
        install(EXPORT SLE${module}${config_name}Targets FILE SLE${module}${config_name}Targets.cmake DESTINATION ${config_package_location})
    endforeach()

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/SLEConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/SLEConfigVersion.cmake" DESTINATION ${config_package_location} COMPONENT devel)
endfunction()

macro(AddLibrary module)

    # parse the arguments
    cmake_parse_arguments(THIS "STATIC" "DEPENDENCIES" "SOURCES" ${ARGN})
    if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Extra unparsed arguments when calling AddLibrary: ${THIS_UNPARSED_ARGUMENTS}")
    endif()

    # create the target
    string(TOLOWER sle-${module} target)
    if(THIS_STATIC)
        add_library(${target} STATIC ${THIS_SOURCES})
    else()
        add_library(${target} ${THIS_SOURCES})
    endif()
    add_library(SLE::${module} ALIAS ${target})

    # enable C++17 support
    target_compile_features(${target} PUBLIC cxx_std_17)

    # Add required flags for GCC if coverage reporting is enabled
    if(ENABLE_COVERAGE AND (COMPILER_GCC OR COMPILER_CLANG))
        target_compile_options(${target} PUBLIC $<$<CONFIG:DEBUG>:-O0> $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:DEBUG>:-fprofile-arcs> $<$<CONFIG:DEBUG>:-ftest-coverage>)
        target_link_options(${target} PUBLIC $<$<CONFIG:DEBUG>:--coverage>)
    endif()

    set_target_warnings(${target})
    SetPublicSymbolsHidden(${target})

    # enable precompiled headers
    if (ENABLE_PCH AND (NOT ${target} STREQUAL "SLE-Common"))
        message(VERBOSE "enabling PCH for SLE library '${target}'")
        target_precompile_headers(${target} REUSE_FROM SLE-Common)
    endif()

    # define the export symbol of the module
    string(REPLACE "-" "_" NAME_UPPER "${target}")
    string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL ${NAME_UPPER}_EXPORTS)

    # define the export name of the module
    set_target_properties(${target} PROPERTIES EXPORT_NAME SLE::${module})

    # adjust the output file prefix/suffix to match our conventions
    if(BUILD_SHARED_LIBS AND NOT THIS_STATIC)
        if(PLATFORM_WINDOWS)
            # include the major version number in Windows shared library names (but not import library names)
            set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
            set_target_properties(${target} PROPERTIES SUFFIX "${PROJECT_VERSION_MAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}")

            # fill out all variables we use to generate the .rc file
            string(TIMESTAMP RC_CURRENT_YEAR "%Y")
            string(REGEX REPLACE "SLE-([a-z])([a-z]*)" "\\1" RC_MODULE_NAME_HEAD "${target}")
            string(REGEX REPLACE "SLE-([a-z])([a-z]*)" "\\2" RC_MODULE_NAME_TAIL "${target}")
            string(TOUPPER "${RC_MODULE_NAME_HEAD}" RC_MODULE_NAME_HEAD)
            set(RC_MODULE_NAME "${RC_MODULE_NAME_HEAD}${RC_MODULE_NAME_TAIL}")
            set(RC_VERSION_SUFFIX "") # Add something like the git revision short SHA-1 in the future
            set(RC_PRERELEASE "0") # Set to 1 to mark the DLL as a pre-release DLL
            set(RC_TARGET_NAME "${target}")
            set(RC_TARGET_FILE_NAME_SUFFIX "-${PROJECT_VERSION_MAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}")

            # generate the .rc file
            configure_file(
                "${PROJECT_SOURCE_DIR}/tools/windows/resource.rc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc"
                @ONLY
            )
            target_sources(${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc")
            source_group("" FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc")

            if(COMPILER_GCC OR COMPILER_CLANG)
                # on Windows + gcc/clang get rid of "lib" prefix for shared libraries,
                # and transform the ".dll.a" suffix into ".a" for import libraries
                set_target_properties(${target} PROPERTIES PREFIX "")
                set_target_properties(${target} PROPERTIES IMPORT_SUFFIX ".a")
            endif()
        else()
            set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
        endif()
    else()
        set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -s-d)
        set_target_properties(${target} PROPERTIES RELEASE_POSTFIX -s)
        set_target_properties(${target} PROPERTIES MINSIZEREL_POSTFIX -s)
        set_target_properties(${target} PROPERTIES RELWITHDEBINFO_POSTFIX -s)
    endif()

    # set the version and soversion of the target (for compatible systems -- mostly Linuxes)
    # except for Android which strips soversion suffixes
    if(NOT PLATFORM_ANDROID)
        #set_target_properties(${target} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
        #set_target_properties(${target} PROPERTIES VERSION ${PROJECT_VERSION})
    endif()

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "")

    # set the target flags to use the appropriate C++ standard library
    SetSTDLib(${target})

    # For Visual Studio on Windows, export debug symbols (PDB files) to lib directory
    if(GENERATE_PDB)
        # PDB files are only generated in Debug and RelWithDebInfo configurations, find out which one
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            set(PDB_POSTFIX "-d")
        else()
            set(PDB_POSTFIX "")
        endif()

        if(BUILD_SHARED_LIBS AND NOT THIS_STATIC)
            # DLLs export debug symbols in the linker PDB (the compiler PDB is an intermediate file)
            set_target_properties(${target} PROPERTIES
                                  PDB_NAME "${target}${PDB_POSTFIX}"
                                  PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
        else()
            if(NOT ${target} STREQUAL "SLE-main")
                string(PREPEND PDB_POSTFIX "-s")
            endif()

            if(ENABLE_PCH)
                message(VERBOSE "overriding PDB name for '${target}' with \"SLE-Common\" due to PCH being enabled")

                # For PCH builds with PCH reuse, the PDB name must be the same as the target that's being reused
                set_target_properties(${target} PROPERTIES
                                      COMPILE_PDB_NAME "SLE-Common"
                                      COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
            else()
                # Static libraries have no linker PDBs, thus the compiler PDBs are relevant
                set_target_properties(${target} PROPERTIES
                                      COMPILE_PDB_NAME "${target}${PDB_POSTFIX}"
                                      COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
            endif()
        endif()
    endif()


    if(PLATFORM_ANDROID)
        set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()

    if(BUILD_SHARED_LIBS)
        set(config_name "Shared")
    else()
        set(config_name "Static")
    endif()

    install(TARGETS ${target} EXPORT SLE${module}${config_name}Targets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT bin
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
            FRAMEWORK DESTINATION "." COMPONENT bin)

    
    if(INSTALL_PKGCONFIG_FILES)
        configure_file(
            "${PROJECT_SOURCE_DIR}/tools/pkg-config/${target}.pc.in"
            "${CMAKE_CURRENT_BINARY_DIR}/tools/pkg-config/${target}.pc"
            @ONLY)
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tools/pkg-config/${target}.pc"
            DESTINATION "${PKGCONFIG_INSTALL_DIR}")
    endif()

    
    get_property(ADD_LIBRARY_MODULES GLOBAL PROPERTY ADD_LIBRARY_MODULES_PROPERTY)
    list(APPEND ADD_LIBRARY_MODULES ${module})
    set_property(GLOBAL PROPERTY ADD_LIBRARY_MODULES_PROPERTY "${ADD_LIBRARY_MODULES}")

    
    if(NOT BUILD_SHARED_LIBS AND THIS_DEPENDENCIES)
        
        include(CMakePackageConfigHelpers)

        configure_package_config_file("${THIS_DEPENDENCIES}" "${CMAKE_CURRENT_BINARY_DIR}/SLE${module}Dependencies.cmake"
            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/SLE")

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${module}Dependencies.cmake"
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
                COMPONENT devel)
    endif()

    # add <project>/include as public include directory
    target_include_directories(${target}
                               PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                               PRIVATE ${PROJECT_SOURCE_DIR}/src)

    if(BUILD_FRAMEWORKS)
        target_include_directories(${target} INTERFACE $<INSTALL_INTERFACE:SLE.framework>)
    else()
        target_include_directories(${target} INTERFACE $<INSTALL_INTERFACE:include>)
    endif()

    # define STATIC if the build type is not set to 'shared'
    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${target} PUBLIC "SLE_STATIC")
    endif()

    message(STATUS "AddLibrary - ${target}")


endmacro()

