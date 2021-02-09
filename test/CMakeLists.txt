# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
	message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

include(GoogleTest)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
	include_directories("${gtest_SOURCE_DIR}/include")
endif()

add_executable(run-tests
    main.cpp
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

set(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ca-bundle.crt
)

set_target_properties(run-tests PROPERTIES
    RESOURCE ${RESOURCE_FILES}
	XCODE_GENERATE_SCHEME TRUE
	XCODE_SCHEME_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

target_include_directories(run-tests
    PUBLIC "${gtest_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC "${PROJECT_SOURCE_DIR}/src"
    PUBLIC "${PROJECT_SOURCE_DIR}/cpp-httplib"
    PUBLIC "${PROJECT_SOURCE_DIR}/json/include"
    PUBLIC "${PROJECT_SOURCE_DIR}/uriparser/include"
)

target_link_libraries(run-tests uriparser)
target_link_libraries(run-tests httplib::httplib)
target_link_libraries(run-tests cxxopts::cxxopts)
target_link_libraries(run-tests nlohmann_json::nlohmann_json)
target_link_libraries(run-tests gtest_main)

gtest_discover_tests(run-tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)