cmake_minimum_required(VERSION 3.1.3)

include(CheckCXXCompilerFlag)

project(Gsx2Jsonpp VERSION 2.1.0)

# Set minimum C++ to 2011 standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# From http://www.cmake.org/pipermail/cmake/2010-March/035992.html:
# function to collect all the sources from sub-directories
# into a single list
function(add_sources)
    get_property(is_defined GLOBAL PROPERTY SRCS_LIST DEFINED)
    if(NOT is_defined)
      define_property(GLOBAL PROPERTY SRCS_LIST
        BRIEF_DOCS "List of source files"
        FULL_DOCS "List of all source files in the entire project")
    endif()
    # make absolute paths
    set(SRCS)
    foreach(s IN LISTS ARGN)
      if(NOT IS_ABSOLUTE "${s}")
          get_filename_component(s "${s}" ABSOLUTE)
      endif()
          list(APPEND SRCS "${s}")
    endforeach()
    # append to global list
    set_property(GLOBAL APPEND PROPERTY SRCS_LIST "${SRCS}")
endfunction(add_sources)

###################################################
### General install settings
###################################################

option(${PROJECT_NAME}_BUILD_TESTS "Build tests" ON)
option(${PROJECT_NAME}_BUILD_BENCHMARK_TESTS "Build benchmark" ON)

set(SERVER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/gsx2json.cpp
)
set(SERVER_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/gsx2json.h
)
set(RESOURCE_FILES
    cpp-httplib/example/ca-bundle.crt
)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(${PROJECT_NAME}
	src/main.cpp
	${SERVER_SOURCES}
	${SERVER_HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RESOURCE "${RESOURCE_FILES}"
	XCODE_GENERATE_SCHEME TRUE
	XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/json/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/cxxopts/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/uriparser/include"
)

set(CMAKE_MACOSX_RPATH ON)
set(BUILD_SHARED_LIBS  OFF CACHE BOOL "")
set(URIPARSER_ENABLE_INSTALL OFF CACHE BOOL "")
set(URIPARSER_BUILD_TOOLS OFF CACHE BOOL "")
set(URIPARSER_BUILD_TESTS OFF CACHE BOOL "")
set(URIPARSER_BUILD_DOCS OFF CACHE BOOL "")

add_subdirectory(json)
add_subdirectory(cxxopts)
add_subdirectory(uriparser)
add_subdirectory(cpp-httplib)

target_link_libraries(${PROJECT_NAME} uriparser)
target_link_libraries(${PROJECT_NAME} httplib::httplib)
target_link_libraries(${PROJECT_NAME} cxxopts::cxxopts)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

###################################################
### OpenSSL libraries
###################################################
find_package(OpenSSL REQUIRED)
if (OpenSSL_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
    if (VERBOSE)
      message(STATUS "OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR}")
    endif()
    target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
endif()

###################################################
### Zlib libraries
###################################################
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${ZLIB_INCLUDE_DIRS})
    if (VERBOSE)
      message(STATUS "ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS}")
    endif()
    target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
endif()

###################################################
### Installation
###################################################

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    RESOURCE DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

if (MSVC AND _VCPKG_INSTALLED_DIR)
    install(DIRECTORY "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}$<$<CONFIG:Debug>:/debug>/bin/"
            DESTINATION bin
            FILES_MATCHING PATTERN "*.dll" )
endif()

###################################################
### Testing
###################################################

if (${PROJECT_NAME}_BUILD_BENCHMARK_TESTS)
    add_subdirectory(benchmark)
endif()

if (${PROJECT_NAME}_BUILD_TESTS)
    enable_testing ()
    add_subdirectory(test)
endif()
